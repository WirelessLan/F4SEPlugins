#pragma once

struct WeaponData {
	TESForm* item;					// 00
	TBO_InstanceData* instanceData;	// 08
};

typedef std::uint32_t(*_UseAmmo)(Actor* actor, const WeaponData& a_weapon, std::uint32_t a_equipIndex, std::uint32_t a_shotCount);

class BSAnimationGraphEvent {
public:
	TESObjectREFR*	refr;
	BSFixedString	name;
	BSFixedString	args;
};

typedef EventResult(*_PlayerAnimGraphEvent_ReceiveEvent)(void*, BSAnimationGraphEvent*, void*);

void Install_Hooks();