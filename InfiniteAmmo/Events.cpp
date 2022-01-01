#include "Global.h"

EventResult ItemEquipEvent::ReceiveEvent(TESEquipEvent* evn, void* dispatcher) {
	if (evn->isEquipping) {
		Actor::MiddleProcess::Data08::EquipData* equipData = GetEquipDataByFormID(evn->formId);
		if (equipData) 
			CheckAmmo(equipData->item, (TESObjectWEAP::InstanceData*)equipData->instanceData, GetEquipIndex(equipData));
	}

	return kEvent_Continue;
}

void Install_Events() {
	GetEventDispatcher<TESEquipEvent>()->AddEventSink(new ItemEquipEvent());
}