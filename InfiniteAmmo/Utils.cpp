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

	tArray<Actor::MiddleProcess::Data08::EquipData> equipDataArray = reinterpret_cast<tArray<Actor::MiddleProcess::Data08::EquipData>&>((*g_player)->middleProcess->unk08->equipData);
	if (equipDataArray.count == 0)
		return nullptr;

	return &reinterpret_cast<tArray<Actor::MiddleProcess::Data08::EquipData>&>((*g_player)->middleProcess->unk08->equipData);
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
		UInt32 eIdx = reinterpret_cast<UInt32&>(equipDataArray->entries[ii].unk18);
		if (eIdx == equipIndex)
			return &equipDataArray->entries[ii];
	}

	return nullptr;
}

UInt16 GetCurrentAmmoCapacity() {
	const tArray<Actor::MiddleProcess::Data08::EquipData>* equipDataArray = GetEquipDataArray();
	if (!equipDataArray) 
		return 0;

	for (UInt32 ii = 0; ii < equipDataArray->count; ii++) {
		if (!IsThrowableWeapon(reinterpret_cast<UInt32&>(equipDataArray->entries[ii].unk18)))
			return GetCurrentAmmoCapacity(GetWeaponInstanceData(equipDataArray->entries[ii].item, equipDataArray->entries[ii].instanceData));
	}

	return 0;
}

UInt16 GetCurrentAmmoCapacity(TESObjectWEAP::InstanceData* weapInst) {
	if (!weapInst)
		return 0;
	return GetAmmoCapacity(weapInst, weapInst->ammoCapacity);
}

UInt16 GetAmmoCapacity(TESObjectWEAP::InstanceData* weapInst, UInt16 weapAmmoCap) {
	if (!weapInst)
		return 0;

	// 탄약의 타입을 체크
	UInt16 ammoType = GetAmmoType(weapInst, weapAmmoCap);

	// 현재 무기의 장전가능한 탄환 수량 조회
	UInt16 ammoCapacity = 0;
	// 일반 무기의 경우 기존의 장전가능한 탄환 수량을 이용하면 됨
	if (ammoType == AmmoType::kAmmoType_Default || ammoType == AmmoType::kAmmoType_Charging)
		ammoCapacity = weapAmmoCap;
	// 끝없는 무기의 경우 장전가능 탄환 수량 0
	else if (ammoType & AmmoType::kAmmoType_NeverEnding)
		ammoCapacity = 0;
	// 퓨전코어 무기의 경우 무조건 퓨전코어 1개
	else if (ammoType & AmmoType::kAmmoType_FusionCore)
		ammoCapacity = 1;

	return ammoCapacity;
}

TESObjectWEAP::InstanceData* GetWeaponInstanceData(TESForm* weapForm, TBO_InstanceData* weapInst) {
	TESObjectWEAP::InstanceData* weapInstData = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(weapInst, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
	if (weapInstData)
		return weapInstData;

	TESObjectWEAP* objWeap = DYNAMIC_CAST(weapForm, TESForm, TESObjectWEAP);
	if (!objWeap)
		return nullptr;

	return &objWeap->weapData;
}

UInt32 GetInventoryItemCount(Actor* actor, TESForm* item) {
	if (!actor || !item)
		return 0;

	BGSInventoryList* inventory = actor->inventoryList;
	if (!inventory)
		return 0;

	UInt32 totalItemCount = 0;
	inventory->inventoryLock.LockForRead();
	for (UInt32 ii = 0; ii < inventory->items.count; ii++) {
		if (inventory->items[ii].form == item) {
			BGSInventoryItem::Stack* sp = inventory->items[ii].stack;
			while (sp) {
				totalItemCount += sp->count;
				sp = sp->next;
			}
			break;
		}
	}
	inventory->inventoryLock.Unlock();

	return totalItemCount;
}

bool IsThrowableWeapon(UInt32 equipIndex) {
	return equipIndex == EquipIndex::kEquipIndex_Throwable;
}

inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr) {
	UInt32 formID = std::stoul(formIdStr, nullptr, 16) & 0xFFFFFF;
	return GetFormFromIdentifier(pluginName, formID);
}

TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId) {
	if (!*g_dataHandler)
		return nullptr;

	const ModInfo* mod = (*g_dataHandler)->LookupModByName(pluginName.c_str());
	if (!mod || mod->modIndex == -1)
		return nullptr;

	UInt32 actualFormId = formId;
	UInt32 pluginIndex = mod->GetPartialIndex();
	if (!mod->IsLight())
		actualFormId |= pluginIndex << 24;
	else
		actualFormId |= pluginIndex << 12;

	return LookupFormByID(actualFormId);
}

void ShowMessagebox(std::string asText) {
	CallGlobalFunctionNoWait1<BSFixedString>("Debug", "Messagebox", BSFixedString(asText.c_str()));
}