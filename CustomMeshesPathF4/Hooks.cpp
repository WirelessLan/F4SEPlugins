#include "Hooks.h"

#include <thread>
#include <mutex>
#include <unordered_map>

#include <f4se/GameRTTI.h>
#include <f4se/GameReferences.h>
#include <f4se/NiNodes.h>
#include <f4se/NiExtraData.h>
#include <f4se/BSModelDB.h>
#include <f4se/xbyak/xbyak.h>
#include <f4se_common/BranchTrampoline.h>

#include "CACS.h"
#include "Utils.h"

namespace Hooks {
	template <class _key, class _value>
	class ThreadSafeMap {
	public:
		ThreadSafeMap() {}
		void Add(_key key, _value value) {
			std::lock_guard<std::mutex> guard(_mutex);
			_map[key] = value;
		}
		_value* Get(const _key& key) {
			std::lock_guard<std::mutex> guard(_mutex);
			auto it = _map.find(key);
			if (it == _map.end())
				return nullptr;
			return &it->second;
		}
		void Delete(_key key) {
			std::lock_guard<std::mutex> guard(_mutex);
			_map.erase(key);
		}
		void Clear() {
			std::lock_guard<std::mutex> guard(_mutex);
			_map.clear();
		}
	private:
		std::unordered_map<_key, _value> _map;
		std::mutex _mutex;
	};

	struct FormPath {
		std::uint32_t FormID;
		std::string Path;
	};

	struct CustomPath {
		FormPath ActorPath;
		FormPath RacePath;
	};

	ThreadSafeMap<std::thread::id, CustomPath> g_threadPathMap;
	ThreadSafeMap<std::string, std::string> g_fullPathMap;

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

		std::uint32_t baseFormId = Utils::GetBaseFormID(actor);
		std::uint32_t raceFormId = actor->race ? actor->race->formID : 0;

		std::thread::id threadId = std::this_thread::get_id();

		bool isTarget = false;
		if (CACS::CheckCACSRule(CACS::RuleType::kRuleType_Actor, baseFormId) || CACS::CheckCACSRule(CACS::RuleType::kRuleType_Race, raceFormId)) {
			isTarget = true;
			std::string actorPath = CACS::GetCACSPath(CACS::RuleType::kRuleType_Actor, baseFormId);
			std::string racePath = CACS::GetCACSPath(CACS::RuleType::kRuleType_Race, raceFormId);
			g_threadPathMap.Add(threadId, { { baseFormId, actorPath }, { raceFormId, racePath } });
		}

		ReplaceRefModel(a_npc, a_ref);

		if (isTarget)
			g_threadPathMap.Delete(threadId);
	}

	const char* PrepareName_Hook(char* arg1, std::uint32_t arg2, const char* subPath, const char* prefixPath) {
		std::thread::id threadId = std::this_thread::get_id();

		const CustomPath* cPath = g_threadPathMap.Get(threadId);
		if (cPath) {
			if (_stricmp(prefixPath, "meshes\\") == 0 && _stricmp(Utils::GetFileExt(subPath).c_str(), "nif") == 0) {
				std::string customPrefixPath, customSubPath, customPath;

				// Check Actor Path
				if (CACS::SetCustomPaths(CACS::RuleType::kRuleType_Actor, cPath->ActorPath.FormID, cPath->ActorPath.Path, prefixPath, subPath, customPrefixPath, customSubPath, customPath)) {
					g_fullPathMap.Add(customPath, cPath->ActorPath.Path);
					return PrepareName(arg1, arg2, customSubPath.c_str(), customPrefixPath.c_str());
				}

				// Check Race Path
				if (CACS::SetCustomPaths(CACS::RuleType::kRuleType_Race, cPath->RacePath.FormID, cPath->RacePath.Path, prefixPath, subPath, customPrefixPath, customSubPath, customPath)) {
					g_fullPathMap.Add(customPath, cPath->RacePath.Path);
					return PrepareName(arg1, arg2, customSubPath.c_str(), customPrefixPath.c_str());
				}
			}
		}

		return PrepareName(arg1, arg2, subPath, prefixPath);
	}

	class CustomModelProcessor : public BSModelDB::BSModelProcessor {
	public:
		CustomModelProcessor(BSModelDB::BSModelProcessor* oldProcessor) : m_oldProcessor(oldProcessor) { }

		virtual void Process(BSModelDB::ModelData* modelData, const char* modelName, NiAVObject** root, UInt32* typeOut) {
			if (modelName) {
				std::string modelPath = Utils::RemovePrefix(Utils::ReplaceSlash(Utils::ToLower(modelName)), "data\\");
				std::string* cPath = g_fullPathMap.Get(modelPath);
				if (cPath && root && *root) {
					NiAVObject* node = *root;
					node->IncRef();

					NiExtraData* bodyTri = node->GetExtraData("BODYTRI");
					if (bodyTri) {
						bodyTri->IncRef();

						NiStringExtraData* stringData = DYNAMIC_CAST(bodyTri, NiExtraData, NiStringExtraData);
						if (stringData) {
							std::string dataStr = Utils::ReplaceSlash(Utils::ToLower(stringData->m_string.c_str()));
							if (Utils::StartsWith(dataStr, "meshes\\"))
								dataStr = "meshes\\" + *cPath + Utils::RemovePrefix(dataStr, "meshes\\");
							else
								dataStr = *cPath + dataStr;

							stringData->m_string = dataStr.c_str();
						}

						bodyTri->DecRef();
					}

					node->DecRef();
				}
			}

			if (m_oldProcessor)
				m_oldProcessor->Process(modelData, modelName, root, typeOut);
		}

		DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free)

	protected:
		BSModelDB::BSModelProcessor* m_oldProcessor;
	};

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
}