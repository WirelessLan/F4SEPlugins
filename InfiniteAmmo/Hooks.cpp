#include "Global.h"

typedef std::uint32_t(*_UseAmmo)(Actor* actor, const WeaponData& a_weapon, std::uint32_t a_equipIndex, std::uint32_t a_shotCount);
RelocAddr<uintptr_t> UseAmmo_Target(0x2D438E8 + 0x780);
_UseAmmo UseAmmo_Original;

std::uint32_t UseAmmo_Hook(Actor* actor, const WeaponData& a_weapon, std::uint32_t a_equipIndex, std::uint32_t a_shotCount) {
	std::uint32_t retVal = UseAmmo_Original(actor, a_weapon, a_equipIndex, a_shotCount);
	if (actor == *g_player)
		CheckAmmo(a_weapon.item, (TESObjectWEAP::InstanceData*)a_weapon.instanceData, a_equipIndex);
	return retVal;
}

void Install_Hooks() {
	UseAmmo_Original = *(_UseAmmo*)(UseAmmo_Target.GetUIntPtr());
	SafeWrite64(UseAmmo_Target, (uintptr_t)UseAmmo_Hook);
}