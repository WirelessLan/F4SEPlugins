#include "Global.h"

EventResult ItemEquipEvent::ReceiveEvent(TESEquipEvent* evn, void* dispatcher) {
	if (evn->ref == *g_player && evn->isEquipping) {
		Actor::MiddleProcess::Data08::EquipData* equipData = GetEquipDataByFormID(evn->formId);
		if (equipData) {
			TESObjectWEAP::InstanceData* weapInst = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(equipData->instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
			bool isThrowable = IsThrowableWeapon(equipData->unk18);
			if (!isThrowable)
				CurrentAmmoCapacity = GetCurrentAmmoCapacity(equipData->item, weapInst);
			CheckAmmo(equipData->item, weapInst, isThrowable, 0);
		}
	}
	return kEvent_Continue;
}

void Install_Events() {
	GetEventDispatcher<TESEquipEvent>()->AddEventSink(new ItemEquipEvent());
}