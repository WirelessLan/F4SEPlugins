#include "Global.h"

ThreadPathMap g_threadPathMap;
FullPathMap g_fullPathMap;

typedef void (*_ActorChangeMeshes)(void*, Actor*);
RelocAddr <_ActorChangeMeshes> ActorChangeMeshes_HookTarget(0x5B93F0);
_ActorChangeMeshes ActorChangeMeshes_Original;

typedef const char* (*_SetModelPath)(void*, UInt64, const char*, const char*);
RelocAddr <_SetModelPath> SetModelPath_HookTarget(0x1B7CD40);
_SetModelPath SetModelPath_Original;

void ActorChangeMeshes_Hook(void* arg1, Actor* actor) {
	std::thread::id threadId = std::this_thread::get_id();

	UInt32 raceFormId = actor->race ? actor->race->formID : 0xFFFFFFFF;
	UInt32 baseFormId = actor->baseForm ? actor->baseForm->formID : 0xFFFFFFFF;

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

	ActorChangeMeshes_Original(arg1, actor);

	if (isTarget)
		g_threadPathMap.Delete(threadId);
}

const char* SetModelPath_Hook(void* arg1, UInt64 arg2, const char* subPath, const char* prefixPath) {
	std::thread::id threadId = std::this_thread::get_id();

	const CustomPath* paths = g_threadPathMap.Get(threadId);
	if (paths) {
		if (_stricmp(prefixPath, "meshes\\") == 0 && _stricmp(GetFileExt(subPath).c_str(), "nif") == 0) {
			std::string prefixPathStr = toLower(prefixPath);
			std::string subPathStr = toLower(subPath);

			subPathStr = remove_prefix(prefixPathStr, subPathStr);

			std::string customPrefixPath, customSubPath, customFullPath;

			// Check Actor Path
			if (SetCustomPaths(RuleType::kRuleType_Actor, paths->actorId, paths->actorPath, subPathStr, customPrefixPath, customSubPath, customFullPath)) {
				g_fullPathMap.Add(customFullPath, *paths);
				return SetModelPath_Original(arg1, arg2, customSubPath.c_str(), customPrefixPath.c_str());
			}

			// Check Race Path
			if (SetCustomPaths(RuleType::kRuleType_Race, paths->raceId, paths->racePath, subPathStr, customPrefixPath, customSubPath, customFullPath)) {
				g_fullPathMap.Add(customFullPath, *paths);
				return SetModelPath_Original(arg1, arg2, customSubPath.c_str(), customPrefixPath.c_str());
			}
		}
	}

	return SetModelPath_Original(arg1, arg2, subPath, prefixPath);
}

void CustomModelProcessor::Process(BSModelDB::ModelData* modelData, const char* modelName, NiAVObject** root, UInt32* typeOut) {
	CustomPath paths;
	if (g_fullPathMap.Get(modelName, paths)) {
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

void Hooks_ActorChangeMeshes() {
	struct ActorChangeMeshes_Code : Xbyak::CodeGenerator {
		ActorChangeMeshes_Code(void* buf) : Xbyak::CodeGenerator(4096, buf)	{
			Xbyak::Label retnLabel;

			mov(ptr[rsp + 0x10], rdx);
			mov(ptr[rsp + 0x08], rcx);

			jmp(ptr[rip + retnLabel]);

			L(retnLabel);
			dq(ActorChangeMeshes_HookTarget.GetUIntPtr() + 0x0A);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	ActorChangeMeshes_Code code(codeBuf);
	g_localTrampoline.EndAlloc(code.getCurr());

	ActorChangeMeshes_Original = (_ActorChangeMeshes)codeBuf;

	g_branchTrampoline.Write6Branch(ActorChangeMeshes_HookTarget.GetUIntPtr(), (uintptr_t)ActorChangeMeshes_Hook);
}

void Hooks_SetModelPath() {
	struct SetModelPath_Code : Xbyak::CodeGenerator {
		SetModelPath_Code(void* buf) : Xbyak::CodeGenerator(4096, buf)	{
			Xbyak::Label retnLabel;

			mov(ptr[rsp + 0x08], rbx);
			mov(ptr[rsp + 0x10], rbp);
			mov(ptr[rsp + 0x18], rsi);

			jmp(ptr[rip + retnLabel]);

			L(retnLabel);
			dq(SetModelPath_HookTarget.GetUIntPtr() + 0x0F);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	SetModelPath_Code code(codeBuf);
	g_localTrampoline.EndAlloc(code.getCurr());

	SetModelPath_Original = (_SetModelPath)codeBuf;

	g_branchTrampoline.Write6Branch(SetModelPath_HookTarget.GetUIntPtr(), (uintptr_t)SetModelPath_Hook);
}

void ClearPathMap() {
	g_threadPathMap.Clear();
	g_fullPathMap.Clear();
}