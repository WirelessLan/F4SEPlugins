#include "Global.h"
#include "MathUtils.h"

Actor* g_selectedActor;
NiNode* g_selectedNode;
std::unordered_map<std::uint32_t, std::unordered_map<std::string, Nodes::NodeData>> g_modifiedMap;

namespace Nodes {
	Actor* GetActor(std::uint32_t a_formID) {
		TESForm* form = LookupFormByID(a_formID);
		if (!form)
			return nullptr;

		Actor* actor = DYNAMIC_CAST(form, TESForm, Actor);
		if (!actor)
			return nullptr;

		return actor;
	}

	NiNode* GetNode(Actor* a_actor, const std::string& a_nodeName) {
		if (a_nodeName.empty())
			return nullptr;

		NiNode* rootNode = a_actor->GetActorRootNode(false);
		if (!rootNode) 
			return nullptr;

		NiAVObject* avObject = rootNode->GetObjectByName(&BSFixedString(a_nodeName.c_str()));
		if (!avObject)
			return nullptr;

		NiNode* node = avObject->GetAsNiNode();
		if (!node)
			return nullptr;

		return node;
	}

	NodeData* GetNodeData(Actor* a_actor, const std::string& a_nodeName) {
		auto map_it = g_modifiedMap.find(a_actor->formID);
		if (map_it == g_modifiedMap.end())
			return nullptr;

		auto node_it = map_it->second.find(a_nodeName);
		if (node_it == map_it->second.end())
			return nullptr;

		return &node_it->second;
	}

	NodeData* InsertNodeData(Actor* a_actor, const std::string& a_nodeName, const NodeData& a_nodeData) {
		return &(g_modifiedMap[a_actor->formID][a_nodeName] = a_nodeData);
	}

	void InsertModMap(Actor* a_actor) {
		g_modifiedMap[a_actor->formID];
	}

	void SetNodes(Actor* a_actor, const std::unordered_map<std::string, Nodes::NodeData>& a_nodeDataMap) {
		if (!a_actor)
			return;

		for (auto& nodeDataPair : a_nodeDataMap) {
			const std::string& nodeName = nodeDataPair.first;
			const NodeData& setNodeData = nodeDataPair.second;

			if (!setNodeData.posModified && !setNodeData.rotModified && !setNodeData.scaleModified)
				continue;

			NiNode* node = GetNode(a_actor, nodeName);
			if (!node)
				continue;

			NodeData* nodeData = GetNodeData(a_actor, nodeName);
			if (!nodeData)
				nodeData = InsertNodeData(a_actor, nodeName, setNodeData);
			else
				*nodeData = setNodeData;

			if (setNodeData.posModified) {
				node->m_localTransform.pos.x = setNodeData.pos.x;
				node->m_localTransform.pos.y = setNodeData.pos.y;
				node->m_localTransform.pos.z = setNodeData.pos.z;
			}
			if (setNodeData.rotModified) {
				node->m_localTransform.rot.SetEulerAngles(setNodeData.rot.y, setNodeData.rot.p, setNodeData.rot.r);
			}
			if (setNodeData.scaleModified) {
				node->m_localTransform.scale = setNodeData.scale;
			}
		}
	}

	void ModNode(ModType a_modType, ModDirection a_modDir, bool a_isPositive) {
		if (!g_selectedActor || !g_selectedNode)
			return;

		std::string nodeName = g_selectedNode->m_name;

		NodeData* nodeData = GetNodeData(g_selectedActor, nodeName);
		if (!nodeData) {
			Nodes::NodeData nNodeData = {};
			nodeData = InsertNodeData(g_selectedActor, nodeName, nNodeData);
			if (!nodeData)
				return;
		}

		// Save Original NodeData
		if (a_modType == ModType::kModType_Transition && !nodeData->posModified) {
			nodeData->posModified = true;
			nodeData->pos.x = g_selectedNode->m_localTransform.pos.x;
			nodeData->pos.y = g_selectedNode->m_localTransform.pos.y;
			nodeData->pos.z = g_selectedNode->m_localTransform.pos.z;
		}
		else if (a_modType == ModType::kModType_Rotation && !nodeData->rotModified) {
			float y_org, p_org, r_org;
			g_selectedNode->m_localTransform.rot.GetEulerAngles(&y_org, &p_org, &r_org);

			nodeData->rotModified = true;
			nodeData->rot.y = y_org;
			nodeData->rot.p = p_org;
			nodeData->rot.r = r_org;
		}
		else if (a_modType == ModType::kModType_Scale && !nodeData->scaleModified) {
			nodeData->scaleModified = true;
			nodeData->scale = g_selectedNode->m_localTransform.scale;
		}

		if (a_modType == ModType::kModType_Transition) {
			float value = a_isPositive ? 0.1 : -0.1;

			if (a_modDir == ModDirection::kModDirection_X)
				g_selectedNode->m_localTransform.pos.x += value;
			else if (a_modDir == ModDirection::kModDirection_Y)
				g_selectedNode->m_localTransform.pos.y += value;
			else if (a_modDir == ModDirection::kModDirection_Z)
				g_selectedNode->m_localTransform.pos.z += value;
		}
		else if (a_modType == ModType::kModType_Rotation) {
			float value = a_isPositive ? 0.01 : -0.01;

			RE::NiMatrix3 rot = *(RE::NiMatrix3 *)&g_selectedNode->m_localTransform.rot;

			NiPoint3CLib rollAxis = ToDirectionVector(rot);
			NiPoint3CLib yawAxis = ToUpVector(rot);
			NiPoint3CLib pitchAxis = CrossProduct(rollAxis, yawAxis);
			if (a_modDir == ModDirection::kModDirection_X)
				rot = rot * GetRotationMatrix33(yawAxis, value);
			else if (a_modDir == ModDirection::kModDirection_Y)
				rot = rot * GetRotationMatrix33(pitchAxis, value);
			else if (a_modDir == ModDirection::kModDirection_Z)
				rot = rot * GetRotationMatrix33(rollAxis, value);

			g_selectedNode->m_localTransform.rot = *(NiMatrix43*)&rot;
		}
		else if (a_modType == ModType::kModType_Scale) {
			float value = a_isPositive ? 0.01 : -0.01;
			g_selectedNode->m_localTransform.scale += value;
		}
	}

	void ResetNode(Actor* a_actor, const std::string& a_nodeName, NodeData* a_originData) {
		NiNode* node = GetNode(a_actor, a_nodeName);
		if (!node)
			return;

		if (a_originData->posModified) {
			node->m_localTransform.pos.x = a_originData->pos.x;
			node->m_localTransform.pos.y = a_originData->pos.y;
			node->m_localTransform.pos.z = a_originData->pos.z;
		}
		if (a_originData->rotModified) {
			node->m_localTransform.rot.SetEulerAngles(a_originData->rot.y, a_originData->rot.p, a_originData->rot.r);
		}
		if (a_originData->scaleModified) {
			node->m_localTransform.scale = a_originData->scale;
		}

		auto map_it = g_modifiedMap.find(a_actor->formID);
		if (map_it == g_modifiedMap.end())
			return;

		auto node_it = map_it->second.find(a_nodeName);
		if (node_it == map_it->second.end())
			return;

		map_it->second.erase(a_nodeName);

		if (map_it->second.empty())
			g_modifiedMap.erase(a_actor->formID);
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

	void ClearAll() {
		for (auto map_It : g_modifiedMap) {
			Actor* actor = GetActor(map_It.first);
			if (!actor)
				continue;

			for (auto node_it : map_It.second)
				Nodes::ResetNode(actor, node_it.first, &node_it.second);

			Utility::UnfreezeActor(actor);
		}

		g_modifiedMap.clear();

		ClearSelect();
	}

	void ClearSelect() {
		g_selectedActor = nullptr;
		g_selectedNode = nullptr;
	}
};