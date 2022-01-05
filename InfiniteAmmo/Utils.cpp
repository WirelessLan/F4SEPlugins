#include "Global.h"

RelocAddr <_AddItem> AddItem_Internal(0x4279D0);

void AddItem(TESObjectREFR* refr, TESForm* item, UInt32 count, bool isSilent) {
	if (!refr || !item)
		return;

	if (count == 0)
		return;

	AddItemData addItemData = { refr, 0, isSilent };
	ItemData itemData = { 0 };
	itemData.item = item;
	itemData.count = count;
	itemData.unk28 = 1.0f;

	AddItem_Internal(&addItemData, &itemData);
}

const tArray<Actor::MiddleProcess::Data08::EquipData>* GetEquipDataArray() {
	if (!*g_player || !(*g_player)->middleProcess || !(*g_player)->middleProcess->unk08)
		return nullptr;

	tArray<Actor::MiddleProcess::Data08::EquipData> equipDataArray = reinterpret_cast<tArray<Actor::MiddleProcess::Data08::EquipData> &>((*g_player)->middleProcess->unk08->equipData);
	if (equipDataArray.count == 0)
		return nullptr;

	return &reinterpret_cast<tArray<Actor::MiddleProcess::Data08::EquipData> &>((*g_player)->middleProcess->unk08->equipData);
}

Actor::MiddleProcess::Data08::EquipData* GetEquipDataByFormID(UInt32 formId) {
	const tArray<Actor::MiddleProcess::Data08::EquipData>* equipDataArray = GetEquipDataArray();
	if (!equipDataArray)
		return nullptr;

	for (UInt32 ii = 0; ii < equipDataArray->count; ii++) {
		if (equipDataArray->entries[ii].item->formID == formId)
			return &equipDataArray->entries[ii];
	}

	return nullptr;
}

Actor::MiddleProcess::Data08::EquipData* GetEquipDataByEquipIndex(EquipIndex equipIndex) {
	const tArray<Actor::MiddleProcess::Data08::EquipData>* equipDataArray = GetEquipDataArray();
	if (!equipDataArray)
		return nullptr;

	for (UInt32 ii = 0; ii < equipDataArray->count; ii++) {
		if (equipDataArray->entries[ii].unk18 == equipIndex)
			return &equipDataArray->entries[ii];
	}

	return nullptr;
}

UInt16 GetCurrentAmmoCapacity() {
	const tArray<Actor::MiddleProcess::Data08::EquipData>* equipDataArray = GetEquipDataArray();
	if (!equipDataArray) 
		return 0;

	for (UInt32 ii = 0; ii < equipDataArray->count; ii++) {
		if (!IsThrowableWeapon(equipDataArray->entries[ii].unk18))
			return GetCurrentAmmoCapacity(equipDataArray->entries[ii].item, (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(equipDataArray->entries[ii].instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData));
	}

	return 0;
}

UInt16 GetCurrentAmmoCapacity(TESForm* weap, TESObjectWEAP::InstanceData* weapInst) {
	if (!weapInst) {
		if (!weap)
			return 0;

		TESObjectWEAP* objWeap = DYNAMIC_CAST(weap, TESForm, TESObjectWEAP);
		if (!objWeap)
			return 0;
		weapInst = &objWeap->weapData;
	}
	
	return weapInst->ammoCapacity;
}

bool IsThrowableWeapon(UInt32 equipIndex) {
	return equipIndex == EquipIndex::kEquipIndex_Throwable;
}

static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

template <typename T>
T GetOffset(const void* baseObject, int offset) {
	return *reinterpret_cast<T*>((uintptr_t)baseObject + offset);
}

TESForm* GetFormFromIdentifier(const std::string& identifier) {
	if (!*g_dataHandler)
		return nullptr;

	auto delimiter = identifier.find('|');
	if (delimiter != std::string::npos) {
		std::string modName = identifier.substr(0, delimiter);
		std::string modForm = identifier.substr(delimiter + 1);

		const ModInfo* mod = (*g_dataHandler)->LookupModByName(modName.c_str());
		if (mod && mod->modIndex != -1) {
			UInt32 formID = std::stoul(modForm, nullptr, 16) & 0xFFFFFF;
			UInt32 flags = GetOffset<UInt32>(mod, 0x334);
			if (flags & (1 << 9)) {
				// ESL
				formID &= 0xFFF;
				formID |= 0xFE << 24;
				formID |= GetOffset<UInt16>(mod, 0x372) << 12;	// ESL load order
			}
			else {
				formID |= (mod->modIndex) << 24;
			}
			return LookupFormByID(formID);
		}
	}
	return nullptr;
}