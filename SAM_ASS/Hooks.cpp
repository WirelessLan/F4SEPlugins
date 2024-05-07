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
								_MESSAGE("[Info] SAF_Version NiIntegerExtraData inserted into %s", modelName);
							}
						}
						rootNode->DecRef();
					}

					for (auto nodePair : Configs::g_nodeMap) {
						NiAVObject* insertingNode = (*root)->GetObjectByName(&BSFixedString(nodePair.second.c_str()));
						if (insertingNode)
							continue;

						NiAVObject* targetNode = (*root)->GetObjectByName(&BSFixedString(nodePair.first.c_str()));
						if (!targetNode) {
							_MESSAGE("[Warning] Node %s was not found on skeleton %s", nodePair.first.c_str(), modelName);
							continue;
						}

						NiNode* parentNode = targetNode->m_parent;
						if (!parentNode)
							continue;

						NiNode* newNode = parentNode->Create(0);
						if (!newNode) {
							_MESSAGE("[Warning] Failed to attach new node %s on skeleton %s", nodePair.second.c_str(), modelName);
							continue;
						}

						newNode->m_name = nodePair.second.c_str();

						targetNode->IncRef();

						parentNode->RemoveChild(targetNode);
						parentNode->AttachChild(newNode, true);
						newNode->AttachChild(targetNode, true);

						targetNode->DecRef();

						_MESSAGE("[Info] Node %s was attached to skeleton %s", nodePair.second.c_str(), modelName);
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