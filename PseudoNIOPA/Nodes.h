#pragma once

namespace Nodes {
	enum ModType {
		kModType_Transition,
		kModType_Rotation,
		kModType_Scale
	};

	enum ModDirection {
		kModDirection_X,
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

		std::string nodeName;
		bool posModified = false;
		Position pos;
		bool rotModified = false;
		Rotation rot;
		bool scaleModified = false;
		float scale;
	};

	NiNode* GetNode(Actor* actor, const std::string& nodeName);

	void ModNode(ModType modType, ModDirection modDir, bool isPositive);

	void ResetNode(Actor* actor, const std::string& nodeName, NodeData* originData);
	void ResetSelectedNode();

	NodeData* GetNodeData(Actor* actor, const std::string& nodeName);
	void InsertNodeData(Actor* actor, const std::string& nodeName, NodeData& nodeData);
	void InsertNodeData(Actor* actor, BSFixedString& nodeName, NodeData& nodeData);
	void InsertModMap(Actor* actor);

	Actor* GetActor(UInt32 formId);

	void SetNodes(Actor* actor, const std::unordered_map<std::string, Nodes::NodeData>& nodeDataMap);

	void ClearAll();
	void ClearSelect();
};