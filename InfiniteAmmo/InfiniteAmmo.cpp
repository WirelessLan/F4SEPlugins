#include "Global.h"

UInt16 iNeverEndingCapacity;
UInt32 iMinAmmoCapacityMult;
bool bUseInfiniteAmmo;
bool bUseInfiniteThrowableWeapon;
bool bUseWhiteListMode;
std::unordered_set<UInt32> excludedWeapons;
std::unordered_set<UInt32> includedWeapons;

bool IsExcludedWeapon(UInt32 weapFormId) {
	return excludedWeapons.find(weapFormId) != excludedWeapons.end();
}

bool IsIncludedWeapon(UInt32 weapFormId) {
	return includedWeapons.find(weapFormId) != includedWeapons.end();
}

UInt16 GetAmmoType(TESObjectWEAP::InstanceData* weapInst, UInt16 ammoCapacity) {
	UInt16 ammoType = AmmoType::kAmmoType_Default;

	// 현재 장비한 무기의 최대 장전 가능 탄약량이 0일때: 끝없는 
	if (ammoCapacity == 0)
		ammoType |= AmmoType::kAmmoType_NeverEnding;

	UInt32 ammoHealth = reinterpret_cast<UInt32&>(weapInst->ammo->unk160[1]);
	// 현재 장비한 무기의 탄약의 Health가 0이 아닐때: 퓨전코어
	if (ammoHealth != 0)
		ammoType |= AmmoType::kAmmoType_FusionCore;

	// 현재 장비한 무기의 플래그에 ChargingReload가 있을때: 충전식 장전
	if (weapInst->flags & TESObjectWEAP::InstanceData::WeaponFlags::kFlag_ChargingReload)
		ammoType |= AmmoType::kAmmoType_Charging;

	return ammoType;
}

void AddAmmo(TESForm* weapForm, TESObjectWEAP::InstanceData* weapInst) {
	if (!weapForm || !weapInst)
		return;

	// 탄약 무한 옵션이 꺼져있을 때 무시
	if (!bUseInfiniteAmmo)
		return;

	// 현재 무기가 제외 무기일 경우 무시
	if (IsExcludedWeapon(weapForm->formID))
		return;

	// 화이트리스트 모드가 켜져있고 현재 무기가 화이트리스트에 포함되지 않는 경우 무시
	if (bUseWhiteListMode && !IsIncludedWeapon(weapForm->formID))
		return;

	// 탄약이 존재하지 않는 무기인 경우 무시
	TESForm* ammo = weapInst->ammo;
	if (!ammo)
		return;

	UInt16 ammoCapacity = CurrentAmmoCapacity;

	// 끝없는 무기의 경우 끝없는 무기 기본 탄환 수량을 이용
	if (GetAmmoType(weapInst, ammoCapacity) & AmmoType::kAmmoType_NeverEnding)
		ammoCapacity = iNeverEndingCapacity;

	// 현재 탄환의 총 보유수량 조회
	UInt32 totalAmmoCount = GetInventoryItemCount(*g_player, ammo);

	// 탄약 추가
	if (totalAmmoCount < ammoCapacity * iMinAmmoCapacityMult) {
		UInt32 diff = ammoCapacity * iMinAmmoCapacityMult - totalAmmoCount;
		AddItem(*g_player, ammo, diff, true);
	}
}

bool IsInfiniteThrowable(TESForm* weapForm) {
	if (!weapForm)
		return false;

	// 투척무기 무한이 아닌 경우 유한
	if (!bUseInfiniteThrowableWeapon)
		return false;

	// 제외무기에 포함된 경우 유한
	if (IsExcludedWeapon(weapForm->formID))
		return false;

	// 화이트리스트 모드이고 화이트 리스트 무기가 아닌 경우 유한
	if (bUseWhiteListMode && !IsIncludedWeapon(weapForm->formID))
		return false;

	return true;
}

bool IsNeverEndingWeapon(TESForm* weapForm, TESObjectWEAP::InstanceData* weapInst) {
	if (!weapInst || !weapInst->ammo)
		return false;

	// 탄약무한이 아닌 경우 유한
	if (!bUseInfiniteAmmo)
		return false;

	// 제외무기에 포함된 경우 유한
	if (IsExcludedWeapon(weapForm->formID))
		return false;

	// 화이트리스트 모드이고 화이트 리스트 무기가 아닌 경우 유한
	if (bUseWhiteListMode && !IsIncludedWeapon(weapForm->formID))
		return false;

	return GetAmmoType(weapInst, CurrentAmmoCapacity) & AmmoType::kAmmoType_NeverEnding;
}

char GetNextChar(const std::string& line, UInt32& index) {
	if (index < line.length())
		return line[index++];

	return -1;
}

std::string GetNextData(const std::string& line, UInt32& index, char delimeter) {
	char ch;
	std::string retVal = "";

	while ((ch = GetNextChar(line, index)) > 0) {
		if (ch == '#') {
			if (index > 0) index--;
			break;
		}

		if (delimeter != 0 && ch == delimeter)
			break;

		retVal += ch;
	}

	trim(retVal);
	return retVal;
}

void ReadWeaponsConfigFile() {
	std::string settingFilePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  "_Weapons.cfg" };
	std::ifstream settingFile(settingFilePath);

	excludedWeapons.clear();
	includedWeapons.clear();

	if (!settingFile.is_open()) {
		_MESSAGE("Cannot open a weapon config file!");
		return;
	}

	std::string line;
	std::string lineType, pluginName, formId;
	while (std::getline(settingFile, line)) {
		trim(line);
		if (line.length() == 0 || line[0] == '#')
			continue;

		UInt32 index = 0;

		lineType = GetNextData(line, index, '|');
		if (lineType.length() == 0) {
			_MESSAGE("Cannot read Line Type[%s]", line.c_str());
			continue;
		}

		if (lineType == "Exclude" || lineType == "Include") {
			pluginName = GetNextData(line, index, '|');
			if (pluginName.length() == 0) {
				_MESSAGE("Cannot read Plugin Name[%s]", line.c_str());
				continue;
			}

			formId = GetNextData(line, index, 0);
			if (formId.length() == 0) {
				_MESSAGE("Cannot read Form ID[%s]", line.c_str());
				continue;
			}

			TESForm* weapForm = GetFormFromIdentifier(pluginName, formId);
			if (!weapForm) {
				_MESSAGE("Cannot find Weapon[%s]", line.c_str());
				continue;
			}

			if (lineType == "Exclude")
				excludedWeapons.insert(weapForm->formID);
			else if (lineType == "Include")
				includedWeapons.insert(weapForm->formID);

			_MESSAGE("%s Weapon: %s | 0x%08X", lineType, pluginName.c_str(), weapForm->formID);
		}
		else {
			_MESSAGE("Cannot determine Line Type[%s]", line.c_str());
			continue;
		}
	}

	settingFile.close();
}

void UpdateWeaponsConfigFile() {
	std::string settingFilePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  "_Weapons.cfg" };
	std::ofstream settingFile(settingFilePath);

	if (!settingFile.is_open()) {
		_MESSAGE("Cannot open a plugin setting file!");
		return;
	}

	for (UInt32 element : excludedWeapons) {
		TESForm* weapForm = LookupFormByID(element);
		if (!weapForm)
			continue;

		ModInfo* modInfo = weapForm->unk08->entries[0];

		UInt32 formId = modInfo->IsLight() ? element & 0xFFF : element & 0xFFFFFF;
		settingFile << "Exclude|" << modInfo->name << "|" << std::uppercase << std::hex << formId << std::endl;
	}

	for (UInt32 element : includedWeapons) {
		TESForm* weapForm = LookupFormByID(element);
		if (!weapForm)
			continue;

		ModInfo* modInfo = weapForm->unk08->entries[0];

		UInt32 formId = modInfo->IsLight() ? element & 0xFFF : element & 0xFFFFFF;
		settingFile << "Include|" << modInfo->name << "|" << std::uppercase << std::hex << formId << std::endl;
	}

	settingFile.close();
}

void LoadSettings() {

	// Default Settings
	bUseInfiniteAmmo = true;
	bUseInfiniteThrowableWeapon = true;
	iNeverEndingCapacity = 1;
	iMinAmmoCapacityMult = 2;
	bUseWhiteListMode = false;

	GetConfigValue("Settings", "bUseInfiniteAmmo", &bUseInfiniteAmmo);
	GetConfigValue("Settings", "bUseInfiniteThrowableWeapon", &bUseInfiniteThrowableWeapon);
	GetConfigValue("Settings", "iNeverEndingCapacity", &iNeverEndingCapacity);
	GetConfigValue("Settings", "iMinAmmoCapacityMult", &iMinAmmoCapacityMult);
	GetConfigValue("Settings", "bUseWhiteListMode", &bUseWhiteListMode);

	_MESSAGE("bUseInfiniteAmmo: %u", bUseInfiniteAmmo);
	_MESSAGE("bUseInfiniteThrowableWeapon: %u", bUseInfiniteThrowableWeapon);
	_MESSAGE("iNeverEndingCapacity: %u", iNeverEndingCapacity);
	_MESSAGE("iMinAmmoCapacityMult: %u", iMinAmmoCapacityMult);
	_MESSAGE("bUseWhiteListMode: %u", bUseWhiteListMode);

	ReadWeaponsConfigFile();
}

std::string AddCurrentWeaponToList(bool isExcluded, EquipIndex equipIndex) {
	Actor::MiddleProcess::Data08::EquipData* equipData = GetEquipDataByEquipIndex(equipIndex);
	if (!equipData)
		return "You are not equipped with a weapon";

	if (excludedWeapons.find(equipData->item->formID) != excludedWeapons.end())
		return "It is already included in the excluded weapons list";

	if (includedWeapons.find(equipData->item->formID) != includedWeapons.end())
		return "It is already included in the included weapons list";

	if (isExcluded)
		excludedWeapons.insert(equipData->item->formID);
	else 
		includedWeapons.insert(equipData->item->formID);

	return "";
}

std::string RemoveCurrentWeaponFromList(bool isExcluded, EquipIndex equipIndex) {
	Actor::MiddleProcess::Data08::EquipData* equipData = GetEquipDataByEquipIndex(equipIndex);
	if (!equipData)
		return "You are not equipped with a weapon";

	if (isExcluded) {
		if (excludedWeapons.find(equipData->item->formID) == excludedWeapons.end())
			return "It is not in the excluded weapons list";
		excludedWeapons.erase(equipData->item->formID);
	}
	else {
		if (includedWeapons.find(equipData->item->formID) == includedWeapons.end())
			return "It is not in the included weapons list";
		includedWeapons.erase(equipData->item->formID);
	}

	return "";
}

void InfiniteAmmo_MCMSettings::Invoke(Args* args) {
	if (args->numArgs == 0 || !args->args[0].IsString())
		return;

	if (args->numArgs == 2) {
		if (strcmp(args->args[0].GetString(), "bUseInfiniteAmmo") == 0)
			bUseInfiniteAmmo = args->args[1].GetBool();
		else if (strcmp(args->args[0].GetString(), "bUseInfiniteThrowableWeapon") == 0)
			bUseInfiniteThrowableWeapon = args->args[1].GetBool();
		else if (strcmp(args->args[0].GetString(), "iNeverEndingCapacity") == 0)
			iNeverEndingCapacity = args->args[1].GetInt();
		else if (strcmp(args->args[0].GetString(), "iMinAmmoCapacityMult") == 0)
			iMinAmmoCapacityMult = args->args[1].GetInt();
		else if (strcmp(args->args[0].GetString(), "bUseWhiteListMode") == 0)
			bUseWhiteListMode = args->args[1].GetBool();
	}
	else if (args->numArgs == 1) {
		std::string result;

		if (strcmp(args->args[0].GetString(), "AddWeapon_Excluded") == 0)
			result = AddCurrentWeaponToList(true, EquipIndex::kEquipIndex_Default);
		else if (strcmp(args->args[0].GetString(), "AddThrowable_Excluded") == 0)
			result = AddCurrentWeaponToList(true, EquipIndex::kEquipIndex_Throwable);
		else if (strcmp(args->args[0].GetString(), "RemoveWeapon_Excluded") == 0)
			result = RemoveCurrentWeaponFromList(true, EquipIndex::kEquipIndex_Default);
		else if (strcmp(args->args[0].GetString(), "RemoveThrowable_Excluded") == 0)
			result = RemoveCurrentWeaponFromList(true, EquipIndex::kEquipIndex_Throwable);
		else if (strcmp(args->args[0].GetString(), "ClearExcludedWeapons") == 0)
			excludedWeapons.clear();

		else if (strcmp(args->args[0].GetString(), "AddWeapon_Included") == 0)
			result = AddCurrentWeaponToList(false, EquipIndex::kEquipIndex_Default);
		else if (strcmp(args->args[0].GetString(), "AddThrowable_Included") == 0)
			result = AddCurrentWeaponToList(false, EquipIndex::kEquipIndex_Throwable);
		else if (strcmp(args->args[0].GetString(), "RemoveWeapon_Included") == 0)
			result = RemoveCurrentWeaponFromList(false, EquipIndex::kEquipIndex_Default);
		else if (strcmp(args->args[0].GetString(), "RemoveThrowable_Included") == 0)
			result = RemoveCurrentWeaponFromList(false, EquipIndex::kEquipIndex_Throwable);
		else if (strcmp(args->args[0].GetString(), "ClearIncludedWeapons") == 0)
			includedWeapons.clear();

		if (!result.empty()) {
			ShowMessagebox(result);
			return;
		}

		UpdateWeaponsConfigFile();
	}
}