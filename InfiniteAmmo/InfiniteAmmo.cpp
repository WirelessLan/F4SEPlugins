#include "Global.h"

UInt16 uNeverEndingCapacity;
UInt32 uMinAmmoCapacityMult;
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
		ammoCapacity = uNeverEndingCapacity;

	// 현재 탄환의 총 보유수량 조회
	UInt32 totalAmmoCount = GetInventoryItemCount(*g_player, ammo);

	// 탄약 추가
	if (totalAmmoCount < ammoCapacity * uMinAmmoCapacityMult) {
		UInt32 diff = ammoCapacity * uMinAmmoCapacityMult - totalAmmoCount;
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

void LoadInfiniteAmmoSetting() {
	std::string settingFilePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  ".txt" };
	std::fstream settingFile(settingFilePath);
	if (!settingFile.is_open()) {
		_MESSAGE("Can't open plugin setting file!");
		return;
	}

	uNeverEndingCapacity = 10;
	uMinAmmoCapacityMult = 2;
	bUseInfiniteAmmo = true;
	bUseInfiniteThrowableWeapon = true;
	bUseWhiteListMode = false;

	excludedWeapons.clear();
	includedWeapons.clear();

	std::string line;
	std::string lineType, optionName, optionValueStr, pluginName, formId;
	while (std::getline(settingFile, line)) {
		trim(line);
		if (line.length() == 0 || line[0] == '#')
			continue;

		UInt32 index = 0;

		lineType = GetNextData(line, index, '|');
		if (lineType.length() == 0) {
			_MESSAGE("Can't read Line Type[%s]", line.c_str());
			continue;
		}

		if (lineType == "Option") {
			optionName = GetNextData(line, index, '=');
			if (optionName.length() == 0) {
				_MESSAGE("Can't read Option Name[%s]", line.c_str());
				continue;
			}

			optionValueStr = GetNextData(line, index, 0);
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
			else if (optionName == "bUseWhiteListMode")
				optionValue >> bUseWhiteListMode;
			else {
				_MESSAGE("Unknown Option Name[%s]", line.c_str());
				continue;
			}
		}
		else if (lineType == "Exclude" || lineType == "Include") {
			pluginName = GetNextData(line, index, '|');
			if (pluginName.length() == 0) {
				_MESSAGE("Can't read Plugin Name[%s]", line.c_str());
				continue;
			}

			formId = GetNextData(line, index, 0);
			if (formId.length() == 0) {
				_MESSAGE("Can't read Form ID[%s]", line.c_str());
				continue;
			}

			TESForm* weapForm = GetFormFromIdentifier(pluginName, formId);
			if (!weapForm) {
				_MESSAGE("Can't find Weapon[%s]", line.c_str());
				continue;
			}

			if (lineType == "Exclude")
				excludedWeapons.insert(weapForm->formID);
			else if (lineType == "Include")
				includedWeapons.insert(weapForm->formID);

			_MESSAGE("%s Weapon: %s | 0x%08X", lineType, pluginName, weapForm->formID);
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
	_MESSAGE("bUseWhiteListMode: %u", bUseWhiteListMode);
}