#pragma once

enum AmmoType {
	kAmmoType_Default = 0x00,
	kAmmoType_Charging = 0x01,
	kAmmoType_FusionCore = 0x02,
	kAmmoType_NeverEnding = 0x04
};

bool IsExcludedWeapon(UInt32 weapFormId);

bool IsIncludedWeapon(UInt32 weapFormId);

UInt16 GetAmmoType(TESObjectWEAP::InstanceData* weapInst, UInt16 ammoCapacity);

void AddAmmo(TESForm* weapForm, TESObjectWEAP::InstanceData* weapInst);

bool IsInfiniteThrowable(TESForm* weapForm);

bool IsNeverEndingWeapon(TESForm* weapForm, TESObjectWEAP::InstanceData* weapInst);

void LoadSettings();

class InfiniteAmmo_MCMSettings : public GFxFunctionHandler {
public:
	virtual void Invoke(Args* args);
};