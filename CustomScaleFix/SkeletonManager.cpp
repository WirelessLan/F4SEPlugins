#include "Global.h"

void ModifyScale(Actor* actor, bool isFirstPerson, float scale) {
	NiNode* rootNode = actor->GetActorRootNode(isFirstPerson);
	if (!rootNode)
		return;

	rootNode->m_localTransform.scale = scale;
}