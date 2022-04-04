#include "Global.h"

EventResult ItemEquipEvent::ReceiveEvent(TESEquipEvent* evn, void* dispatcher) {
	if (evn->ref == *g_player && evn->isEquipping) {
		Actor::MiddleProcess::Data08::EquipData* equipData = GetEquipDataByFormID(evn->formId);
		if (equipData) {
			if (!IsThrowableWeapon(reinterpret_cast<UInt32&>(equipData->unk18))) {
				TESObjectWEAP::InstanceData* weapInst = GetWeaponInstanceData(equipData->item, equipData->instanceData);
				CurrentAmmoCapacity = GetCurrentAmmoCapacity(weapInst);
				AddAmmo(equipData->item, weapInst);
			}
		}
	}
	return kEvent_Continue;
}

void Install_Events() {
	GetEventDispatcher<TESEquipEvent>()->AddEventSink(new ItemEquipEvent());
}