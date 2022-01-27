#pragma once

enum AmmoType {
	kAmmoType_Default = 0x00,
	kAmmoType_Changing = 0x01,
	kAmmoType_FusionCore = 0x02,
	kAmmoType_NeverEnding = 0x04
};

bool IsExcludedWeapon(UInt32 weapFormId);

bool IsIncludedWeapon(UInt32 weapFormId);

void CheckAmmo(TESForm* weapForm, TESObjectWEAP::InstanceData* weapInst, UInt32 shotCount, bool forcedReplenishAmmo);

bool IsInfiniteThrowable(TESForm* weapForm);

void LoadInfiniteAmmoSetting();