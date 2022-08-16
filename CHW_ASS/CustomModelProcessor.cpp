#include "Globals.h"

std::vector<WeaponPosition> DefaultAttachPosVec = {
	{
		"WeaponRifleBone",
		-1.467466,
		-1.231682,
		1.663497,
		17.894257,
		-12.836738,
		6.571038,
		"Chest"
	},
	{
		"WeaponPistolUp",
		1.857521,
		1.570796,
		0.000000,
		8.335701,
		-0.659389,
		9.937208,
		"RLeg_Thigh"
	},
	{
		"WeaponMelee2H",
		3.050061,
		-1.185978,
		-1.655665,
		25.776085,
		-12.672893,
		-5.732063,
		"Chest"
	},
	{
		"WeaponMelee1H",
		0.031314,
		-0.449990,
		3.137365,
		-1.815956,
		3.442890,
		14.893623,
		"Pelvis"
	}
};

void SetDataOnNode(NiNode* srcNode, const WeaponPosition& weapPos) {
	if (!srcNode)
		return;

	NiNode* clone = NiNode::Create(0);
	if (!clone)
		return;

	clone->m_name = weapPos.name;
	srcNode->AttachChild(clone, true);

	NiAVObject* attachedBone = srcNode->GetObjectByName(&BSFixedString(weapPos.name));
	if (attachedBone) {
		attachedBone->m_localTransform.rot.SetEulerAngles(weapPos.eulerHeading, weapPos.eulerAttitude, weapPos.eulerBank);
		attachedBone->m_localTransform.pos.x = weapPos.posX;
		attachedBone->m_localTransform.pos.y = weapPos.posY;
		attachedBone->m_localTransform.pos.z = weapPos.posZ;
	}
}

void AttachCustomNodeOnRootNode(NiAVObject* rootNode) {
	if (!rootNode)
		return;

	for each (WeaponPosition var in DefaultAttachPosVec) {
		NiAVObject* par = rootNode->GetObjectByName(&BSFixedString(var.potentialParentNode.c_str()));
		if (par)
			SetDataOnNode(par->GetAsNiNode(), var);
	}
}

void SetModelProcessor() {
	(*g_TESProcessor) = new CustomModelProcessor(*g_TESProcessor);
}

void CustomModelProcessor::Process(BSModelDB::ModelData * modelData, const char * modelName, NiAVObject ** root, UInt32 * typeOut) {
	if (root && *root && modelName) {
		std::string str = std::string(modelName);
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		if (ActorSkeletonPaths.find(str) != ActorSkeletonPaths.end()) {
			if (!(*root)->GetObjectByName(&BSFixedString(DefaultAttachPosVec[0].name)))
				AttachCustomNodeOnRootNode(*root);
		}
	}

	if (m_oldProcessor)
		m_oldProcessor->Process(modelData, modelName, root, typeOut);
}