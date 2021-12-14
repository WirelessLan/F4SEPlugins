#include "Global.h"

bool IsPrewarNora(UInt32 formId, UInt32 baseFormId) {
	// BaseForm의 ID가 일치하는 레퍼런스 NPC가 둘 이상 존재할 수 있으므로
	// 바닐라 레퍼런스 NPC인지 확인하여 무시
	return formId != 0xA7D38 && baseFormId == 0xA7D35;
}

bool IsAAFDoppelganger(UInt32 formId) {
	if (!*g_dataHandler)
		return false;

	static TESForm* aaf_doppForm = GetFormFromIdentifier("AAF.esm|72E2");
	if (!aaf_doppForm)
		return false;
	return formId == aaf_doppForm->formID;
}

bool IsAAFDoppelganger(Actor* actor, bool isFemale) {
	if (!actor || !actor->baseForm)
		return false;

	if (isFemale) {
		if (IsPrewarNora(actor->formID, actor->baseForm->formID) || IsAAFDoppelganger(actor->baseForm->formID))
			return true;
	}
	else if (IsAAFDoppelganger(actor->baseForm->formID))
		return true;

	return false;
}

EventResult SceneActionEventReceiver::ReceiveEvent(TESSceneActionEvent* evn, void* dispatcher) {
	static TESForm* aaf_DoppelgangerWaitQuest = GetFormFromIdentifier("AAF.esm|72E6");
	if (!aaf_DoppelgangerWaitQuest || aaf_DoppelgangerWaitQuest->formID != evn->sceneFormID || evn->actionIndex != 1)
		return kEvent_Continue;

	if (*g_player && (*g_player)->parentCell) {
		for (UInt32 ii = 0; ii < (*g_player)->parentCell->objectList.count; ii++) {
			TESObjectREFR* ref = (*g_player)->parentCell->objectList.entries[ii];
			Actor* actorRef = DYNAMIC_CAST(ref, TESObjectREFR, Actor);
			if (actorRef) {
				bool isPlayerFemale = isActorFemale(*g_player);
				if (IsAAFDoppelganger(actorRef, isPlayerFemale)) {
					SetActorRefScale(actorRef, GetPlayerScale(), isPlayerFemale);
					break;
				}
			}
		}
	}
	return kEvent_Continue;
}