#pragma once

namespace Nodes {
	enum ModType {
		kModType_Transition = 0,
		kModType_Rotation,
		kModType_Scale
	};

	enum ModDirection {
		kModDirection_X = 0,
		kModDirection_Y,
		kModDirection_Z
	};

	struct NodeData {
		struct Position {
			float x;
			float y;
			float z;
		};

		struct Rotation {
			float y;
			float p;
			float r;
		};

		bool posModified = false;
		Position pos;

		bool rotModified = false;
		Rotation rot;

		bool scaleModified = false;
		float scale;
	};

	Actor* GetActor(std::uint32_t a_formID);
	NiNode* GetNode(Actor* a_actor, const std::string& a_nodeName);
	NodeData* GetNodeData(Actor* a_actor, const std::string& a_nodeName);
	NodeData* InsertNodeData(Actor* a_actor, const std::string& a_nodeName, const NodeData& a_nodeData);
	void InsertModMap(Actor* a_actor);

	void SetNodes(Actor* a_actor, const std::unordered_map<std::string, Nodes::NodeData>& a_nodeDataMap);
	void ModNode(ModType a_modType, ModDirection a_modDir, bool a_isPositive);
	void ResetNode(Actor* a_actor, const std::string& a_nodeName, NodeData* a_originData);
	void ResetSelectedNode();

	void ClearAll();
	void ClearSelect();
};