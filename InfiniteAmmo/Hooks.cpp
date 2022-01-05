#include "Global.h"

RelocAddr<uintptr_t> UseAmmo_Target(0x2D438E8 + 0x780);
_UseAmmo UseAmmo_Original;

RelocAddr<uintptr_t> PlayerAnimGraphEvent_ReceiveEvent_Target(0x2D442D8 + 0x8);
_PlayerAnimGraphEvent_ReceiveEvent PlayerAnimationEvent_Original;

std::uint32_t UseAmmo_Hook(Actor* actor, const WeaponData& a_weapon, std::uint32_t a_equipIndex, std::uint32_t a_shotCount) {
	if (actor == *g_player)
		CheckAmmo(a_weapon.item, (TESObjectWEAP::InstanceData*)a_weapon.instanceData, IsThrowableWeapon(a_equipIndex), false, a_shotCount);
	return UseAmmo_Original(actor, a_weapon, a_equipIndex, a_shotCount);
}

EventResult PlayerAnimGraphEvent_ReceiveEvent_Hook(void* arg1, BSAnimationGraphEvent* evn, void* dispatcher) {
	static BSFixedString ReloadComplete("ReloadComplete");
	if (evn->name == ReloadComplete) {
		Actor::MiddleProcess::Data08::EquipData* equipData = GetEquipDataByEquipIndex(EquipIndex::kEquipIndex_Default);
		if (equipData)
			CheckAmmo(equipData->item, (TESObjectWEAP::InstanceData*)equipData->instanceData, IsThrowableWeapon(equipData->unk18), true, 0);
	}

	return PlayerAnimationEvent_Original(arg1, evn, dispatcher);
}

void Install_Hooks() {
	UseAmmo_Original = *(_UseAmmo*)(UseAmmo_Target.GetUIntPtr());
	SafeWrite64(UseAmmo_Target, (uintptr_t)UseAmmo_Hook);

	PlayerAnimationEvent_Original = *(_PlayerAnimGraphEvent_ReceiveEvent*)(PlayerAnimGraphEvent_ReceiveEvent_Target.GetUIntPtr());
	SafeWrite64(PlayerAnimGraphEvent_ReceiveEvent_Target, (uintptr_t)PlayerAnimGraphEvent_ReceiveEvent_Hook);
}