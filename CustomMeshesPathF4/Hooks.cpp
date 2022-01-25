#include "Global.h"

std::unordered_map<std::thread::id, std::pair<std::string, std::string>> g_acThreadMap;

RelocAddr <_ActorChangeMeshes> ActorChangeMeshes_HookTarget(0x5B93F0);
_ActorChangeMeshes ActorChangeMeshes_Original;

RelocAddr <_SetModelPath> SetModelPath_HookTarget(0x1B7CD40);
_SetModelPath SetModelPath_Original;

RelocAddr <uintptr_t> GetNiObject_HookTarget(0x02E14818 + 0x20);
_GetNiObject GetNiObject_Original;

void ActorChangeMeshes_Hook(void* arg1, Actor* arg2) {
	bool isTarget = false;
	std::thread::id threadId = std::this_thread::get_id();

	if (CheckCACSRule(arg2->race->formID, arg2->baseForm->formID)) {
		isTarget = true;
		std::string racePath = GetCACSPath(RuleType::kRuleType_Race, arg2->race->formID);
		std::string actorPath = GetCACSPath(RuleType::kRuleType_Actor, arg2->baseForm->formID);
		g_acThreadMap.insert(std::pair<std::thread::id, std::pair<std::string, std::string>>(threadId, std::pair<std::string, std::string>(racePath, actorPath)));
	}

	ActorChangeMeshes_Original(arg1, arg2);

	if (isTarget)
		g_acThreadMap.erase(threadId);
}

const char* SetModelPath_Hook(void* arg1, UInt64 arg2, const char* subPath, const char* prefixPath) {
	std::thread::id threadId = std::this_thread::get_id();

	auto it = g_acThreadMap.find(threadId);
	if (it != g_acThreadMap.end()) {
		bool checkPath = false;
		size_t subPathLen = _mbstrlen(subPath);
		if (_stricmp(prefixPath, "meshes\\") == 0 && subPathLen >= 4 && _stricmp(&subPath[subPathLen - 4], ".nif") == 0) {
			std::string prefixPathStr = prefixPath;
			std::string subPathStr = subPath;
			std::transform(prefixPathStr.begin(), prefixPathStr.end(), prefixPathStr.begin(), ::tolower);
			std::transform(subPathStr.begin(), subPathStr.end(), subPathStr.begin(), ::tolower);

			size_t prefixPos = subPathStr.find(prefixPathStr);
			if (prefixPos != std::string::npos)
				subPathStr = subPathStr.substr(prefixPos + prefixPathStr.length());

			std::string currentCustomPrefixPath;
			std::string fullPath;
			
			if (it->second.second != "") {
				fullPath = std::string(prefixPath + it->second.second + subPathStr.c_str());
				if (IsFileExists(fullPath)) {
					currentCustomPrefixPath = prefixPath + it->second.second;
					return SetModelPath_Original(arg1, arg2, subPathStr.c_str(), currentCustomPrefixPath.c_str());
				}
			}

			if (it->second.first != "") {
				fullPath = std::string(prefixPath + it->second.first + subPathStr.c_str());
				if (IsFileExists(fullPath)) {
					currentCustomPrefixPath = prefixPath + it->second.first;
					return SetModelPath_Original(arg1, arg2, subPathStr.c_str(), currentCustomPrefixPath.c_str());
				}
			}
		}
	}

	return SetModelPath_Original(arg1, arg2, subPath, prefixPath);
}

void* GetNiObject_Hook(NiAVObject* node) {
	std::thread::id threadId = std::this_thread::get_id();

	auto it = g_acThreadMap.find(threadId);
	if (it != g_acThreadMap.end()) {
		if (node) {
			node->IncRef();
			NiExtraData* bodyMorphs = node->GetExtraData("BODYTRI");
			if (bodyMorphs) {
				NiStringExtraData* stringData = dynamic_cast<NiStringExtraData*>(bodyMorphs);
				if (stringData)	{
					stringData->IncRef();

					bool found = false;
					std::string fullPath, subPath;
					
					if (it->second.second != "") {
						subPath = it->second.second + std::string(stringData->m_string.c_str());
						fullPath = "meshes\\" + subPath;
						if (IsFileExists(fullPath)) {
							found = true;
							stringData->m_string = BSFixedString(subPath.c_str());
						}
					}

					if (!found && it->second.first != "") {
						subPath = it->second.first + std::string(stringData->m_string.c_str());
						fullPath = "meshes\\" + subPath;
						if (IsFileExists(fullPath))
							stringData->m_string = BSFixedString(subPath.c_str());
					}

					stringData->DecRef();
				}
			}
			node->DecRef();
		}
	}

	return GetNiObject_Original(node);
}

void Hooks_ActorChangeMeshes() {
	struct AiProcess_Code : Xbyak::CodeGenerator {
		AiProcess_Code(void* buf) : Xbyak::CodeGenerator(4096, buf)
		{
			Xbyak::Label retnLabel;

			mov(ptr[rsp + 0x10], rdx);
			mov(ptr[rsp + 0x08], rcx);

			jmp(ptr[rip + retnLabel]);

			L(retnLabel);
			dq(ActorChangeMeshes_HookTarget.GetUIntPtr() + 0x0A);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	AiProcess_Code code(codeBuf);
	g_localTrampoline.EndAlloc(code.getCurr());

	ActorChangeMeshes_Original = (_ActorChangeMeshes)codeBuf;

	g_branchTrampoline.Write6Branch(ActorChangeMeshes_HookTarget.GetUIntPtr(), (uintptr_t)ActorChangeMeshes_Hook);
}

void Hooks_SetModelPath() {
	struct AiProcess_Code : Xbyak::CodeGenerator {
		AiProcess_Code(void* buf) : Xbyak::CodeGenerator(4096, buf)
		{
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
	AiProcess_Code code(codeBuf);
	g_localTrampoline.EndAlloc(code.getCurr());

	SetModelPath_Original = (_SetModelPath)codeBuf;

	g_branchTrampoline.Write6Branch(SetModelPath_HookTarget.GetUIntPtr(), (uintptr_t)SetModelPath_Hook);
}

void Hooks_GetNiObject() {
	GetNiObject_Original = *(_GetNiObject*)(GetNiObject_HookTarget.GetUIntPtr());
	SafeWrite64(GetNiObject_HookTarget, (uintptr_t)GetNiObject_Hook);
}