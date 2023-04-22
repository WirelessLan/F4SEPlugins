#include "Global.h"

ThreadPathMap g_threadPathMap;
FullPathMap g_fullPathMap;

using ReplaceRefModel_t = void(*)(TESForm&, TESObjectREFR*);
RelocAddr<ReplaceRefModel_t> ReplaceRefModel_Target(0x5B93F0);
ReplaceRefModel_t ReplaceRefModel;

using PrepareName_t = const char* (*)(char*, std::uint32_t, const char*, const char*);
RelocAddr<PrepareName_t> PrepareName_Target(0x1B7CD40);
PrepareName_t PrepareName;

void ReplaceRefModel_Hook(TESForm& a_npc, TESObjectREFR* a_ref) {
	Actor* actor = DYNAMIC_CAST(a_ref, TESObjectREFR, Actor);
	if (!actor) {
		ReplaceRefModel(a_npc, a_ref);
		return;
	}

	std::thread::id threadId = std::this_thread::get_id();

	std::uint32_t raceFormId = actor->race ? actor->race->formID : 0xFFFFFFFF;
	std::uint32_t baseFormId = actor->baseForm ? actor->baseForm->formID : 0xFFFFFFFF;

	if ((baseFormId >> 24) == 0xFF) {
		TESForm* baseForm = GetActorBaseForm(actor);
		if (baseForm)
			baseFormId = baseForm->formID;
	}

	bool isTarget = false;
	if (CheckCACSRule(RuleType::kRuleType_Race, raceFormId) || CheckCACSRule(RuleType::kRuleType_Actor, baseFormId)) {
		isTarget = true;
		std::string racePath = GetCACSPath(RuleType::kRuleType_Race, raceFormId);
		std::string actorPath = GetCACSPath(RuleType::kRuleType_Actor, baseFormId);
		g_threadPathMap.Add(threadId, { raceFormId, racePath, baseFormId, actorPath });
	}

	ReplaceRefModel(a_npc, a_ref);

	if (isTarget)
		g_threadPathMap.Delete(threadId);
}

const char* PrepareName_Hook(char* arg1, std::uint32_t arg2, const char* subPath, const char* prefixPath) {
	std::thread::id threadId = std::this_thread::get_id();

	const CustomPath* paths = g_threadPathMap.Get(threadId);
	if (paths) {
		if (_stricmp(prefixPath, "meshes\\") == 0 && _stricmp(GetFileExt(subPath).c_str(), "nif") == 0) {
			std::string customPrefixPath, customSubPath, customFullPath;

			// Check Actor Path
			if (SetCustomPaths(RuleType::kRuleType_Actor, paths, prefixPath, subPath, customPrefixPath, customSubPath, customFullPath)) {
				g_fullPathMap.Add(customFullPath, *paths);
				return PrepareName(arg1, arg2, customSubPath.c_str(), customPrefixPath.c_str());
			}

			// Check Race Path
			if (SetCustomPaths(RuleType::kRuleType_Race, paths, prefixPath, subPath, customPrefixPath, customSubPath, customFullPath)) {
				g_fullPathMap.Add(customFullPath, *paths);
				return PrepareName(arg1, arg2, customSubPath.c_str(), customPrefixPath.c_str());
			}
		}
	}

	return PrepareName(arg1, arg2, subPath, prefixPath);
}

void CustomModelProcessor::Process(BSModelDB::ModelData* modelData, const char* modelName, NiAVObject** root, UInt32* typeOut) {
	CustomPath paths;
	if (modelName && g_fullPathMap.Get(modelName, paths)) {
		if (root && *root) {
			NiAVObject* node = *root;
			node->IncRef();

			NiExtraData* bodyTri = node->GetExtraData("BODYTRI");
			if (bodyTri) {
				bodyTri->IncRef();

				NiStringExtraData* stringData = DYNAMIC_CAST(bodyTri, NiExtraData, NiStringExtraData);
				if (stringData) {
					bool found = false;
					std::string dataStr = stringData->m_string.c_str();

					if (!paths.actorPath.empty() && dataStr.find(paths.actorPath) == std::string::npos) {
						std::string subPath = paths.actorPath + dataStr;
						std::string fullPath = "data\\meshes\\" + subPath;
						if (IsFileExists(fullPath)) {
							found = true;
							stringData->m_string = subPath.c_str();
						}
					}


					if (!found && !paths.racePath.empty() && dataStr.find(paths.racePath) == std::string::npos) {
						std::string subPath = paths.racePath + dataStr;
						std::string fullPath = "data\\meshes\\" + subPath;
						if (IsFileExists(fullPath))
							stringData->m_string = subPath.c_str();
					}
				}

				bodyTri->DecRef();
			}

			node->DecRef();
		}
	}

	if (m_oldProcessor)
		m_oldProcessor->Process(modelData, modelName, root, typeOut);
}

void SetModelProcessor() {
	static bool bSet = false;
	if (!bSet) {
		(*g_TESProcessor) = new CustomModelProcessor(*g_TESProcessor);
		bSet = true;
	}
}

void Hooks_ReplaceRefModel() {
	struct asm_code : Xbyak::CodeGenerator {
		asm_code(void* buf) : Xbyak::CodeGenerator(4096, buf) {
			Xbyak::Label retnLabel;

			mov(ptr[rsp + 0x10], rdx);
			mov(ptr[rsp + 0x08], rcx);

			jmp(ptr[rip + retnLabel]);

			L(retnLabel);
			dq(ReplaceRefModel_Target.GetUIntPtr() + 0x0A);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	asm_code code(codeBuf);
	g_localTrampoline.EndAlloc(code.getCurr());

	ReplaceRefModel = (ReplaceRefModel_t)codeBuf;

	g_branchTrampoline.Write6Branch(ReplaceRefModel_Target.GetUIntPtr(), (std::uintptr_t)ReplaceRefModel_Hook);
}

void Hooks_PrepareName() {
	struct asm_code : Xbyak::CodeGenerator {
		asm_code(void* buf) : Xbyak::CodeGenerator(4096, buf) {
			Xbyak::Label retnLabel;

			mov(ptr[rsp + 0x08], rbx);
			mov(ptr[rsp + 0x10], rbp);
			mov(ptr[rsp + 0x18], rsi);

			jmp(ptr[rip + retnLabel]);

			L(retnLabel);
			dq(PrepareName_Target.GetUIntPtr() + 0x0F);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	asm_code code(codeBuf);
	g_localTrampoline.EndAlloc(code.getCurr());

	PrepareName = (PrepareName_t)codeBuf;

	g_branchTrampoline.Write6Branch(PrepareName_Target.GetUIntPtr(), (std::uintptr_t)PrepareName_Hook);
}

void ClearPathMap() {
	g_threadPathMap.Clear();
	g_fullPathMap.Clear();
}