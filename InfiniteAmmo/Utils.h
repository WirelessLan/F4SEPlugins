#pragma once
#include <f4se/GameReferences.h>

enum EquipIndex {
	kEquipIndex_Default = 0,
	kEquipIndex_Throwable = 2
};

struct AddItemData {
	TESObjectREFR* ref;	// 00
	UInt64 unk08;		// 08
	bool silent;		// 10
};

struct ItemData {
	TESForm* item;		// 00
	UInt64	unk08;		// 08
	UInt32	count;		// 10
	UInt32	unk14;		// 14
	UInt64	unk18;		// 18
	UInt64	unk20;		// 20
	float	unk28;		// 28
	UInt32	unk2C;		// 2C
	UInt64	unk30[(0x580 - 0x30) >> 3];	// 30
};

typedef void (*_AddItem)(AddItemData*, ItemData*);
extern RelocAddr <_AddItem> AddItem_Internal;

void AddItem(TESObjectREFR* refr, TESForm* item, UInt32 count, bool isSilent);

Actor::MiddleProcess::Data08::EquipData* GetEquipDataByFormID(UInt32 formId);

Actor::MiddleProcess::Data08::EquipData* GetEquipDataByEquipIndex(EquipIndex equipIndex);

UInt16 GetCurrentAmmoCapacity();

UInt16 GetCurrentAmmoCapacity(TESObjectWEAP::InstanceData* weapInst);

UInt16 GetAmmoCapacity(TESObjectWEAP::InstanceData* weapInst, UInt16 weapAmmoCap);

TESObjectWEAP::InstanceData* GetWeaponInstanceData(TESForm* weapForm, TBO_InstanceData* weapInst);

UInt32 GetInventoryItemCount(Actor* actor, TESForm* item);

bool IsThrowableWeapon(UInt32 equipIndex);

void trim(std::string& s);

TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);

void ShowMessagebox(std::string asText);