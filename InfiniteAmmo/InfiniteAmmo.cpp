#include "Global.h"

UInt32 uNeverEndingCapacity = 10;
UInt32 uMinAmmoCapacityMult = 1;
bool bUseInfiniteAmmo = true;
bool bUseInfiniteThrowableWeapon = true;
std::unordered_set<UInt32> excludedWeapons;

bool IsExcludedWeapon(UInt32 weapFormId) {
	if (excludedWeapons.find(weapFormId) != excludedWeapons.end())
		return true;
	return false;
}

void CheckAmmo(TESForm* weapForm, TESObjectWEAP::InstanceData* weapInst, bool isThrowableWeapon, UInt32 shotCount) {
	if (!isThrowableWeapon && !bUseInfiniteAmmo)
		return;

	if (isThrowableWeapon && !bUseInfiniteThrowableWeapon)
		return;

	if (!weapForm || IsExcludedWeapon(weapForm->formID))
		return;

	if (!weapInst) {
		TESObjectWEAP* objWeap = DYNAMIC_CAST(weapForm, TESForm, TESObjectWEAP);
		if (!objWeap)
			return;
		weapInst = &objWeap->weapData;
	}

	TESForm* ammo = isThrowableWeapon ? weapForm : weapInst->ammo;
	if (!ammo)	// Melee Weapons
		return;

	BGSInventoryList* playerInventory = (*g_player)->inventoryList;
	if (!playerInventory)
		return;

	UInt32 totalAmmoCount = 0;
	playerInventory->inventoryLock.LockForRead();
	for (UInt32 ii = 0; ii < playerInventory->items.count; ii++) {
		if (playerInventory->items[ii].form == ammo) {
			BGSInventoryItem::Stack* sp = playerInventory->items[ii].stack;
			while (sp) {
				totalAmmoCount += sp->count;
				sp = sp->next;
			}
			break;
		}
	}
	playerInventory->inventoryLock.Unlock();

	UInt16 ammoCapacity = 0;
	if (!isThrowableWeapon) {
		UInt32 ammoHealth = static_cast<UInt32>(weapInst->ammo->unk160[1]);
		if (ammoHealth == 0) {
			ammoCapacity = CurrentAmmoCapacity;

			// Never Ending Legendary
			if (ammoCapacity == 0)
				ammoCapacity = uNeverEndingCapacity;
		}
		else	// Fusion Core
			ammoCapacity = 1;
	}
	else
		ammoCapacity = 1;

	// Laser Musket, Salvaged Assaultron Head..
	if (weapInst->flags & TESObjectWEAP::InstanceData::WeaponFlags::kFlag_ChargingReload) {
		Actor::MiddleProcess::Data08::EquipData* equipData = GetEquipDataByFormID(weapForm->formID);
		if (equipData && equipData->equippedData) {
			UInt32 loadedAmmoCount = static_cast<UInt32>(equipData->equippedData->unk18);
			shotCount = loadedAmmoCount;
		}
	}

	UInt32 calculatedTotalAmmoCount = totalAmmoCount > shotCount ? totalAmmoCount - shotCount : 0;
	if (calculatedTotalAmmoCount < ammoCapacity * uMinAmmoCapacityMult) {
		UInt32 diff = ammoCapacity * uMinAmmoCapacityMult - calculatedTotalAmmoCount;
		AddItem(*g_player, ammo, diff, true);
	}
}

void Init_InfiniteAmmo() {
	std::string settingFilePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  ".txt" };
	std::fstream settingFile(settingFilePath);
	if (!settingFile.is_open()) {
		_MESSAGE("Can't open plugin setting file!");
		return;
	}

	std::string line;
	std::string lineType, optionName, optionValueStr, pluginName, formId;
	while (std::getline(settingFile, line)) {
		trim(line);
		if (line.length() == 0 || line[0] == '#')
			continue;

		UInt32 index = 0;
		UInt32 lineLen = line.length();

		lineType = "";
		for (; index < lineLen; index++) {
			if (line[index] == '|') {
				index++;
				break;
			}

			lineType += line[index];
		}
		trim(lineType);
		if (lineType.length() == 0) {
			_MESSAGE("Can't read Line Type[%s]", line.c_str());
			continue;
		}

		if (lineType == "Option") {
			optionName = "";
			for (; index < lineLen; index++) {
				if (line[index] == '=') {
					index++;
					break;
				}

				optionName += line[index];
			}
			trim(optionName);
			if (optionName.length() == 0) {
				_MESSAGE("Can't read Option Name[%s]", line.c_str());
				continue;
			}

			optionValueStr = "";
			for (; index < lineLen; index++)
				optionValueStr += line[index];
			trim(optionValueStr);
			if (optionValueStr.length() == 0) {
				_MESSAGE("Can't read Option Value[%s]", line.c_str());
				continue;
			}

			std::stringstream optionValue(optionValueStr);
			if (optionValue.fail()) {
				_MESSAGE("Can't read Option Value[%s]", line.c_str());
				continue;
			}

			if (optionName == "uNeverEndingCapacity")
				optionValue >> uNeverEndingCapacity;
			else if (optionName == "uMinAmmoCapacityMult")
				optionValue >> uMinAmmoCapacityMult;
			else if (optionName == "bUseInfiniteAmmo")
				optionValue >> bUseInfiniteAmmo;
			else if (optionName == "bUseInfiniteThrowableWeapon")
				optionValue >> bUseInfiniteThrowableWeapon;
			else {
				_MESSAGE("Unknown Option Name[%s]", line.c_str());
				continue;
			}
		}
		else if (lineType == "Exclude") {
			pluginName = "";
			for (; index < lineLen; index++) {
				if (line[index] == '|') {
					index++;
					break;
				}

				pluginName += line[index];
			}
			trim(pluginName);
			if (pluginName.length() == 0) {
				_MESSAGE("Can't read Plugin Name[%s]", line.c_str());
				continue;
			}

			formId = "";
			for (; index < lineLen; index++)
				formId += line[index];
			trim(formId);
			if (formId.length() == 0) {
				_MESSAGE("Can't read Form ID[%s]", line.c_str());
				continue;
			}

			std::string weapFormId = pluginName + "|" + formId;
			TESForm* weapForm = GetFormFromIdentifier(weapFormId);
			if (!weapForm) {
				_MESSAGE("Can't find Weapon[%s]", line.c_str());
				continue;
			}

			excludedWeapons.insert(weapForm->formID);
		}
		else {
			_MESSAGE("Can't determine Line Type[%s]", line.c_str());
			continue;
		}
	}

	settingFile.close();

	_MESSAGE("uNeverEndingCapacity: %u", uNeverEndingCapacity);
	_MESSAGE("uMinAmmoCapacityMult: %u", uMinAmmoCapacityMult);
	_MESSAGE("bUseInfiniteAmmo: %u", bUseInfiniteAmmo);
	_MESSAGE("bUseInfiniteThrowableWeapon: %u", bUseInfiniteThrowableWeapon);
}