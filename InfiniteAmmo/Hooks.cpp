#include "Global.h"

RelocAddr<uintptr_t> UseAmmo_Target(0x2D438E8 + 0x780);
_UseAmmo UseAmmo_Original;

RelocAddr<uintptr_t> PlayerAnimGraphEvent_ReceiveEvent_Target(0x2D442D8 + 0x8);
_PlayerAnimGraphEvent_ReceiveEvent PlayerAnimationEvent_Original;

std::uint32_t UseAmmo_Hook(Actor* actor, const WeaponData& a_weapon, std::uint32_t a_equipIndex, std::uint32_t a_shotCount) {
	if (actor == *g_player) {
		if (IsThrowableWeapon(a_equipIndex)) {
			if (IsInfiniteThrowable(a_weapon.item))
				return GetInventoryItemCount(actor, a_weapon.item);
		} 
		else {
			TESObjectWEAP::InstanceData* weapInst = GetWeaponInstanceData(a_weapon.item, a_weapon.instanceData);
			if (IsNeverEndingWeapon(a_weapon.item, weapInst))
				return GetInventoryItemCount(actor, weapInst->ammo);
		}
	}
	return UseAmmo_Original(actor, a_weapon, a_equipIndex, a_shotCount);
}

EventResult PlayerAnimGraphEvent_ReceiveEvent_Hook(void* arg1, BSAnimationGraphEvent* evn, void* dispatcher) {
	static BSFixedString ReloadComplete("ReloadComplete");
	if (evn->name == ReloadComplete) {
		Actor::MiddleProcess::Data08::EquipData* equipData = GetEquipDataByEquipIndex(EquipIndex::kEquipIndex_Default);
		if (equipData)
			AddAmmo(equipData->item, GetWeaponInstanceData(equipData->item, equipData->instanceData));
	}

	return PlayerAnimationEvent_Original(arg1, evn, dispatcher);
}

void Install_Hooks() {
	UseAmmo_Original = *(_UseAmmo*)(UseAmmo_Target.GetUIntPtr());
	SafeWrite64(UseAmmo_Target, (uintptr_t)UseAmmo_Hook);

	PlayerAnimationEvent_Original = *(_PlayerAnimGraphEvent_ReceiveEvent*)(PlayerAnimGraphEvent_ReceiveEvent_Target.GetUIntPtr());
	SafeWrite64(PlayerAnimGraphEvent_ReceiveEvent_Target, (uintptr_t)PlayerAnimGraphEvent_ReceiveEvent_Hook);
}