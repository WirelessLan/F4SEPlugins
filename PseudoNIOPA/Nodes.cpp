#include "Global.h"
#include "MathUtils.h"

std::unordered_map<UInt32, std::unordered_map<std::string, Nodes::NodeData>> g_modifiedMap;
Actor* g_selectedActor;
NiNode* g_selectedNode;

namespace Nodes {
	NiNode* GetNode(Actor* actor, const std::string& nodeName) {
		if (nodeName.empty())
			return nullptr;

		NiNode* rootNode = actor->GetActorRootNode(false);
		if (!rootNode) 
			return nullptr;

		NiAVObject* avObject = rootNode->GetObjectByName(&BSFixedString(nodeName.c_str()));
		if (!avObject)
			return nullptr;

		NiNode* node = avObject->GetAsNiNode();
		if (!node)
			return nullptr;

		return node;
	}

	void ModNode(ModType modType, ModDirection modDir, bool isPositive) {
		if (!g_selectedActor || !g_selectedNode)
			return;

		std::string nodeName = g_selectedNode->m_name;

		NodeData* nodeData = GetNodeData(g_selectedActor, nodeName);
		if (!nodeData) {
			Nodes::NodeData nNodeData = {};
			nNodeData.nodeName = nodeName;

			if (modType == ModType::kModType_Transition) {
				nNodeData.posModified = true;
				nNodeData.pos.x = g_selectedNode->m_localTransform.pos.x;
				nNodeData.pos.y = g_selectedNode->m_localTransform.pos.y;
				nNodeData.pos.z = g_selectedNode->m_localTransform.pos.z;
			}
			else if (modType == ModType::kModType_Rotation) {
				float y_org, p_org, r_org;
				g_selectedNode->m_localTransform.rot.GetEulerAngles(&y_org, &p_org, &r_org);

				nNodeData.rotModified = true;
				nNodeData.rot.y = y_org;
				nNodeData.rot.p = p_org;
				nNodeData.rot.r = r_org;
			}
			else if (modType == ModType::kModType_Scale) {
				nNodeData.scaleModified = true;
				nNodeData.scale = g_selectedNode->m_localTransform.scale;
			}

			InsertNodeData(g_selectedActor, g_selectedNode->m_name, nNodeData);
		}
		else {
			if (modType == ModType::kModType_Transition && !nodeData->posModified) {
				nodeData->posModified = true;
				nodeData->pos.x = g_selectedNode->m_localTransform.pos.x;
				nodeData->pos.y = g_selectedNode->m_localTransform.pos.y;
				nodeData->pos.z = g_selectedNode->m_localTransform.pos.z;
			}
			else if (modType == ModType::kModType_Rotation && !nodeData->rotModified) {
				float y_org, p_org, r_org;
				g_selectedNode->m_localTransform.rot.GetEulerAngles(&y_org, &p_org, &r_org);

				nodeData->rotModified = true;
				nodeData->rot.y = y_org;
				nodeData->rot.p = p_org;
				nodeData->rot.r = r_org;
			}
			else if (modType == ModType::kModType_Scale && !nodeData->scaleModified) {
				nodeData->scaleModified = true;
				nodeData->scale = g_selectedNode->m_localTransform.scale;
			}
		}

		if (modType == ModType::kModType_Transition) {
			float value = isPositive ? 0.1 : -0.1;

			if (modDir == ModDirection::kModDirection_X)
				g_selectedNode->m_localTransform.pos.x += value;
			else if (modDir == ModDirection::kModDirection_Y)
				g_selectedNode->m_localTransform.pos.y += value;
			else if (modDir == ModDirection::kModDirection_Z)
				g_selectedNode->m_localTransform.pos.z += value;
		}
		else if (modType == ModType::kModType_Rotation) {
			float value = isPositive ? 0.01 : -0.01;

			using RE::NiMatrix3;
			nodeData = GetNodeData(g_selectedActor, nodeName);
			NiMatrix3 rot = *(NiMatrix3 *)&g_selectedNode->m_localTransform.rot;

			NiPoint3CLib rollAxis = ToDirectionVector(rot);
			NiPoint3CLib yawAxis = ToUpVector(rot);
			NiPoint3CLib pitchAxis = CrossProduct(rollAxis, yawAxis);
			if (modDir == ModDirection::kModDirection_X)
				rot = rot * GetRotationMatrix33(yawAxis, value);
			else if (modDir == ModDirection::kModDirection_Y)
				rot = rot * GetRotationMatrix33(pitchAxis, value);
			else if (modDir == ModDirection::kModDirection_Z)
				rot = rot * GetRotationMatrix33(rollAxis, value);

			g_selectedNode->m_localTransform.rot = *(NiMatrix43*)&rot;
		}
		else if (modType == ModType::kModType_Scale) {
			float value = isPositive ? 0.01 : -0.01;
			g_selectedNode->m_localTransform.scale += value;
		}
	}

	void ResetNode(Actor* actor, const std::string& nodeName, NodeData* originData) {
		NiNode* node = GetNode(actor, nodeName);
		if (!node)
			return;

		if (originData->posModified) {
			node->m_localTransform.pos.x = originData->pos.x;
			node->m_localTransform.pos.y = originData->pos.y;
			node->m_localTransform.pos.z = originData->pos.z;
		}
		if (originData->rotModified) {
			node->m_localTransform.rot.SetEulerAngles(originData->rot.y, originData->rot.p, originData->rot.r);
		}
		if (originData->scaleModified) {
			node->m_localTransform.scale = originData->scale;
		}

		auto map_it = g_modifiedMap.find(actor->formID);
		if (map_it == g_modifiedMap.end())
			return;

		auto node_it = map_it->second.find(nodeName);
		if (node_it == map_it->second.end())
			return;

		map_it->second.erase(nodeName);

		if (map_it->second.size() == 0)
			g_modifiedMap.erase(actor->formID);
	}

	void ResetSelectedNode() {
		if (!g_selectedActor || !g_selectedNode)
			return;

		std::string nodeName = g_selectedNode->m_name;
		NodeData *nodeData = GetNodeData(g_selectedActor, nodeName);
		if (!nodeData)
			return;

		ResetNode(g_selectedActor, nodeName, nodeData);
	}

	NodeData* GetNodeData(Actor* actor, const std::string& nodeName) {
		auto map_it = g_modifiedMap.find(actor->formID);
		if (map_it == g_modifiedMap.end())
			return nullptr;

		auto node_it = map_it->second.find(nodeName);
		if (node_it == map_it->second.end())
			return nullptr;

		return &node_it->second;
	}

	void InsertNodeData(Actor* actor, const std::string& nodeName, NodeData& nodeData) {
		auto map_it = g_modifiedMap.find(actor->formID);
		if (map_it == g_modifiedMap.end()) {
			std::unordered_map<std::string, Nodes::NodeData> nNodeMap;
			nNodeMap.insert(std::make_pair(nodeName, nodeData));

			g_modifiedMap.insert(std::make_pair(actor->formID, nNodeMap));
		}
		else {
			map_it->second.insert(std::make_pair(nodeName, nodeData));
		}
	}

	void InsertNodeData(Actor* actor, BSFixedString& nodeName, NodeData& nodeData) {
		return InsertNodeData(actor, std::string(nodeName.c_str()), nodeData);
	}

	void InsertModMap(Actor* actor) {
		auto map_it = g_modifiedMap.find(actor->formID);
		if (map_it != g_modifiedMap.end())
			return;

		std::unordered_map<std::string, Nodes::NodeData> nNodeMap;
		g_modifiedMap.insert(std::make_pair(actor->formID, nNodeMap));
	}

	Actor* GetActor(UInt32 formId) {
		TESForm* form = LookupFormByID(formId);
		if (!form)
			return nullptr;

		Actor* actor = DYNAMIC_CAST(form, TESForm, Actor);
		if (!actor)
			return nullptr;

		return actor;
	}

	void SetNodes(Actor* actor, const std::unordered_map<std::string, Nodes::NodeData>& nodeDataMap) {
		if (!actor)
			return;

		for (auto iter : nodeDataMap) {
			if (!iter.second.posModified && !iter.second.rotModified && !iter.second.scaleModified)
				continue;

			NiNode* node = GetNode(actor, iter.second.nodeName);
			if (!node)
				continue;

			NodeData* nodeData = GetNodeData(actor, iter.second.nodeName);
			if (!nodeData) {
				NodeData newNode = {};
				newNode.nodeName = iter.second.nodeName;

				if (iter.second.posModified) {
					newNode.posModified = true;
					newNode.pos.x = node->m_localTransform.pos.x;
					newNode.pos.y = node->m_localTransform.pos.y;
					newNode.pos.z = node->m_localTransform.pos.z;
				}
				if (iter.second.rotModified) {
					float y, p, r;

					newNode.rotModified = true;
					node->m_localTransform.rot.GetEulerAngles(&y, &p, &r);
					newNode.rot.y = y;
					newNode.rot.p = p;
					newNode.rot.r = r;
				}
				if (iter.second.scaleModified) {
					newNode.scaleModified = true;
					newNode.scale = node->m_localTransform.scale;
				}

				InsertNodeData(actor, iter.second.nodeName, newNode);
			}
			else {
				if (iter.second.posModified) {
					nodeData->posModified = true;
					nodeData->pos.x = node->m_localTransform.pos.x;
					nodeData->pos.y = node->m_localTransform.pos.y;
					nodeData->pos.z = node->m_localTransform.pos.z;
				}
				if (iter.second.rotModified) {
					float y, p, r;

					nodeData->rotModified = true;
					node->m_localTransform.rot.GetEulerAngles(&y, &p, &r);
					nodeData->rot.y = y;
					nodeData->rot.p = p;
					nodeData->rot.r = r;
				}
				if (iter.second.scaleModified) {
					nodeData->scaleModified = true;
					nodeData->scale = node->m_localTransform.scale;
				}
			}

			if (iter.second.posModified) {
				node->m_localTransform.pos.x = iter.second.pos.x;
				node->m_localTransform.pos.y = iter.second.pos.y;
				node->m_localTransform.pos.z = iter.second.pos.z;
			}
			if (iter.second.rotModified) {
				node->m_localTransform.rot.SetEulerAngles(iter.second.rot.y, iter.second.rot.p, iter.second.rot.r);
			}
			if (iter.second.scaleModified) {
				node->m_localTransform.scale = iter.second.scale;
			}
		}
	}

	void ClearAll() {
		for (auto map_It : g_modifiedMap) {
			Actor* actor = Nodes::GetActor(map_It.first);
			if (!actor)
				continue;

			for (auto node_it : map_It.second)
				Nodes::ResetNode(actor, node_it.first, &node_it.second);

			Utility::UnfreezeActor(actor);
		}
		g_modifiedMap.clear();
		g_pressedButtons.clear();

		ClearSelect();
	}

	void ClearSelect() {
		g_selectedActor = nullptr;
		g_selectedNode = nullptr;
	}
};