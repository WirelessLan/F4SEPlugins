#include "Global.h"

void ModifyScale(Actor* actor, bool isFirstPerson, float scale) {
	NiNode* rootNode = actor->GetActorRootNode(isFirstPerson);
	if (!rootNode)
		return;

	rootNode->m_localTransform.scale = scale;
}

void UpdateBBX(Actor* actor, float scale) {
	NiNode* pRootNode3rd = actor->GetActorRootNode(false);
	if (!pRootNode3rd)
		return;

	static BSFixedString bbxStr("BBX");
	BSBound* bbx = (BSBound*)pRootNode3rd->GetExtraData(bbxStr);
	if (!bbx) 
		return;

	float value = 64.0f * scale;
	bbx->center.z = value;
	bbx->extents.z = value;
}