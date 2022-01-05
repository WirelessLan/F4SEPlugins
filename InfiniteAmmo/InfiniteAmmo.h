#pragma once

enum AmmoType {
	kAmmoType_Default = 0x00,
	kAmmoType_Changing = 0x01,
	kAmmoType_FusionCore = 0x02,
	kAmmoType_NeverEnding = 0x04
};

void CheckAmmo(TESForm* weapForm, TESObjectWEAP::InstanceData* weapInst, bool isThrowableWeapon, bool forcedReplenishAmmo, UInt32 shotCount);

void Init_InfiniteAmmo();