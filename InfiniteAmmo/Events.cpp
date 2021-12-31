#include "Global.h"

typedef EventResult(*_PlayerAnimGraphEvent_ReceiveEvent)(void*, BSAnimationGraphEvent*, void*);
RelocAddr<uintptr_t> PlayerAnimGraphEvent_ReceiveEvent_Target(0x2D442D8 + 0x8);
_PlayerAnimGraphEvent_ReceiveEvent PlayerAnimationEvent_Original;

EventResult PlayerAnimGraphEvent_ReceiveEvent_Hook(void* arg1, BSAnimationGraphEvent* evn, void* dispatcher) {
	static BSFixedString reloadStateEnter("reloadStateEnter");
	static BSFixedString WeaponFire("WeaponFire");

	if (evn->name == reloadStateEnter)
		CheckAmmo(0, false, false);
	else if (evn->name == WeaponFire) {
		static BSFixedString throwableWeaponFireArgs("2");
		bool isThrowable = evn->args == throwableWeaponFireArgs;
		CheckAmmo(0, true, isThrowable);
	}

	return PlayerAnimationEvent_Original(arg1, evn, dispatcher);
}

EventResult ItemEquipEvent::ReceiveEvent(TESEquipEvent* evn, void* dispatcher) {
	if (evn->isEquipping)
		CheckAmmo(evn->formId, false, false);

	return kEvent_Continue;
}

void Install_Events() {
	GetEventDispatcher<TESEquipEvent>()->AddEventSink(new ItemEquipEvent());

	PlayerAnimationEvent_Original = *(_PlayerAnimGraphEvent_ReceiveEvent*)(PlayerAnimGraphEvent_ReceiveEvent_Target.GetUIntPtr());
	SafeWrite64(PlayerAnimGraphEvent_ReceiveEvent_Target, (uintptr_t)PlayerAnimGraphEvent_ReceiveEvent_Hook);
}