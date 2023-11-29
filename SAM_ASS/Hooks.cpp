#include "Hooks.h"

#include <algorithm>

#include <f4se/GameAPI.h>
#include <f4se/GameTypes.h>
#include <f4se/BSModelDB.h>
#include <f4se/NiNodes.h>
#include <f4se/NiExtraData.h>

#include "Configs.h"
#include "Utils.h"

namespace Hooks {
	constexpr int safVersion = 2;

	RelocAddr<uintptr_t> s_NiIntegerExtraDataVtbl(0x02E16748);

	NiIntegerExtraData* CreateNiIntegerExtraData(const BSFixedString& name, int value) {
		void* memory = Heap_Allocate(sizeof(NiIntegerExtraData));
		memset(memory, 0, sizeof(NiIntegerExtraData));
		((UInt64*)memory)[0] = s_NiIntegerExtraDataVtbl.GetUIntPtr();
		NiIntegerExtraData* data = (NiIntegerExtraData*)memory;
		data->m_name = name;
		data->value = value;
		return data;
	}

	class CustomModelProcessor : public BSModelDB::BSModelProcessor {
	public:
		CustomModelProcessor(BSModelDB::BSModelProcessor* oldProcessor) : m_oldProcessor(oldProcessor) { }

		virtual void Process(BSModelDB::ModelData* modelData, const char* modelName, NiAVObject** root, UInt32* typeOut) {
			static BSFixedString rootNodeName("Root");
			static BSFixedString safVersionName("SAF_Version");

			if (root && *root && modelName) {
				(*root)->IncRef();
				std::string s_modelName = Utils::NormalizePath(Utils::ToLower(modelName));

				if (Configs::g_actorSkeletonPaths.find(s_modelName) != Configs::g_actorSkeletonPaths.end()) {
					NiAVObject* rootNode = (*root)->GetObjectByName(&rootNodeName);
					if (rootNode) {
						rootNode->IncRef();
						NiIntegerExtraData* extraData = (NiIntegerExtraData*)rootNode->GetExtraData(safVersionName);
						if (!extraData) {							
							extraData = CreateNiIntegerExtraData(safVersionName, safVersion);
							if (extraData) {
								rootNode->AddExtraData(extraData);
								_MESSAGE("[Info] SAF_Version NiIntegerExtraData inserted into %s", s_modelName.c_str());
							}
						}
						rootNode->DecRef();
					}
				}

				(*root)->DecRef();
			}

			if (m_oldProcessor)
				m_oldProcessor->Process(modelData, modelName, root, typeOut);
		}

		DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free)

	protected:
		BSModelDB::BSModelProcessor* m_oldProcessor;
	};

	void Install() {
		(*g_TESProcessor) = new CustomModelProcessor(*g_TESProcessor);
	}
}