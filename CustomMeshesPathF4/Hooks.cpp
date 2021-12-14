#include "Hooks.h"
#include "Utils.h"
#include "CACS.h"

#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"
#include "f4se/NiExtraData.h"
#include "f4se/xbyak/xbyak.h"

#include "f4se_common/Relocation.h"
#include "f4se_common/BranchTrampoline.h"
#include "f4se_common/SafeWrite.h"

#include <unordered_map>
#include <thread>

typedef void (*_ActorChangeAC)(void*, Actor*);
RelocAddr <_ActorChangeAC> ActorChangeAC_HookTarget(0x5B93F0);
_ActorChangeAC ActorChangeAC_Original;

typedef const char* (*_SetModelPath)(void*, UInt64, const char*, const char*);
RelocAddr <_SetModelPath> SetModelPath_HookTarget(0x1B7CD40);
_SetModelPath SetModelPath_Original;

typedef void* (*_GetNiObject)(NiAVObject *);
RelocAddr <uintptr_t> GetNiObject_HookTarget(0x02E14818 + 0x20);
_GetNiObject GetNiObject_Original;

std::unordered_map<std::thread::id, std::pair<std::string, std::string>> g_acThreadMap;

static void ActorChangeAC_Hook(void* arg1, Actor* arg2) {
	bool isTarget = false;
	std::thread::id threadId = std::this_thread::get_id();

	if (CheckCACSRule(arg2->race->formID, arg2->baseForm->formID)) {
		isTarget = true;
		std::string racePath = GetCACSPath(RaceType, arg2->race->formID);
		std::string actorPath = GetCACSPath(ActorType, arg2->baseForm->formID);
		g_acThreadMap.insert(std::pair<std::thread::id, std::pair<std::string, std::string>>(threadId, std::pair<std::string, std::string>(racePath, actorPath)));
	}

	ActorChangeAC_Original(arg1, arg2);

	if (isTarget)
		g_acThreadMap.erase(threadId);
}

static inline BOOL IsFileExists(const std::string& path) {
	if (path == "")
		return false;

	std::string fullPath = "Data\\" + path;

	DWORD dwAttrib = GetFileAttributes(fullPath.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

static const char* SetModelPath_Hook(void* arg1, UInt64 arg2, const char* subPath, const char* prefixPath) {
	std::thread::id threadId = std::this_thread::get_id();

	auto it = g_acThreadMap.find(threadId);
	if (it != g_acThreadMap.end()) {
		bool checkPath = false;
		size_t subPathLen = _mbstrlen(subPath);
		if (_stricmp(prefixPath, "meshes\\") == 0 && _stricmp(&subPath[subPathLen - 4], ".nif") == 0)
			checkPath = true;

		if (checkPath) {
			std::string prefixPathStr = prefixPath;
			std::string subPathStr = subPath;
			std::transform(prefixPathStr.begin(), prefixPathStr.end(), prefixPathStr.begin(), ::tolower);
			std::transform(subPathStr.begin(), subPathStr.end(), subPathStr.begin(), ::tolower);

			size_t prefixPos = subPathStr.find(prefixPathStr);
			if (prefixPos != std::string::npos)
				subPathStr = subPathStr.substr(prefixPos + prefixPathStr.length());

			std::string currentCustomPrefixPath;
			std::string fullPath;
			
			fullPath = std::string(prefixPath + it->second.second + subPathStr.c_str());
			if (IsFileExists(fullPath)) {
				currentCustomPrefixPath = prefixPath + it->second.second;
				return SetModelPath_Original(arg1, arg2, subPathStr.c_str(), currentCustomPrefixPath.c_str());
			}

			fullPath = std::string(prefixPath + it->second.first + subPathStr.c_str());
			if (IsFileExists(fullPath)) {
				currentCustomPrefixPath = prefixPath + it->second.first;
				return SetModelPath_Original(arg1, arg2, subPathStr.c_str(), currentCustomPrefixPath.c_str());
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
				if (stringData)
				{
					stringData->IncRef();

					bool found = false;
					std::string fullPath, subPath;
					
					subPath = it->second.second + std::string(stringData->m_string.c_str());
					fullPath = "meshes\\" + subPath;
					if (IsFileExists(fullPath)) {
						found = true;
						stringData->m_string = BSFixedString(subPath.c_str());
					}

					if (!found) {
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

void Hooks_ActorChangeAC() {
	struct AiProcess_Code : Xbyak::CodeGenerator {
		AiProcess_Code(void* buf) : Xbyak::CodeGenerator(4096, buf)
		{
			Xbyak::Label retnLabel;

			mov(ptr[rsp + 0x10], rdx);
			mov(ptr[rsp + 0x08], rcx);

			jmp(ptr[rip + retnLabel]);

			L(retnLabel);
			dq(ActorChangeAC_HookTarget.GetUIntPtr() + 0x0A);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	AiProcess_Code code(codeBuf);
	g_localTrampoline.EndAlloc(code.getCurr());

	ActorChangeAC_Original = (_ActorChangeAC)codeBuf;

	g_branchTrampoline.Write6Branch(ActorChangeAC_HookTarget.GetUIntPtr(), (uintptr_t)ActorChangeAC_Hook);
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
	GetNiObject_Original = *(_GetNiObject*)(GetNiObject_HookTarget.GetUIntPtr());;
	SafeWrite64(GetNiObject_HookTarget, (uintptr_t)GetNiObject_Hook);
}