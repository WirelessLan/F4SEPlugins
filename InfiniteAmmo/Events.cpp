#include "Global.h"

EventResult ItemEquipEvent::ReceiveEvent(TESEquipEvent* evn, void* dispatcher) {
	if (evn->ref == *g_player && evn->isEquipping) {
		Actor::MiddleProcess::Data08::EquipData* equipData = GetEquipDataByFormID(evn->formId);
		if (equipData) {
			if (!IsThrowableWeapon(equipData->unk18)) {
				TESObjectWEAP::InstanceData* weapInst = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(equipData->instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
				CurrentAmmoCapacity = GetCurrentAmmoCapacity(equipData->item, weapInst);
				CheckAmmo(equipData->item, weapInst, 0, true);
			}
		}
	}
	return kEvent_Continue;
}

void Install_Events() {
	GetEventDispatcher<TESEquipEvent>()->AddEventSink(new ItemEquipEvent());
}