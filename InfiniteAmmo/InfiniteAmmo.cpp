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

void CheckAmmo(TESForm* weapForm, TESObjectWEAP::InstanceData* weapInst, UInt32 shotCount, bool forcedReplenishAmmo) {
	// 탄약 무한 옵션이 꺼져있을 때 무시
	if (!bUseInfiniteAmmo)
		return;

	// 현재 무기가 제외 무기일 경우 무시
	if (!weapForm || IsExcludedWeapon(weapForm->formID))
		return;

	if (!weapInst) {
		TESObjectWEAP* objWeap = DYNAMIC_CAST(weapForm, TESForm, TESObjectWEAP);
		if (!objWeap)
			return;
		weapInst = &objWeap->weapData;
	}

	// 탄약이 존재하지 않는 무기인 경우 무시
	TESForm* ammo = weapInst->ammo;
	if (!ammo)	// Melee Weapons
		return;

	// 탄약의 타입을 체크
	UInt16 ammoType = AmmoType::kAmmoType_Default;

	// 현재 장비한 무기의 최대 장전 가능 탄약량이 0일때: 끝없는 
	if (CurrentAmmoCapacity == 0)
		ammoType |= AmmoType::kAmmoType_NeverEnding;

	UInt32 ammoHealth = static_cast<UInt32>(weapInst->ammo->unk160[1]);
	// 현재 장비한 무기의 탄약의 Health가 0이 아닐때: 퓨전코어
	if (ammoHealth != 0)
		ammoType |= AmmoType::kAmmoType_FusionCore;

	// 현재 장비한 무기의 플래그에 ChargingReload가 있을때: 충전식 장전
	if (weapInst->flags & TESObjectWEAP::InstanceData::WeaponFlags::kFlag_ChargingReload)
		ammoType |= AmmoType::kAmmoType_Changing;

	// 현재 장전되어있는 탄약의 수를 구함
	UInt32 loadedAmmoCount = 0;
	Actor::MiddleProcess::Data08::EquipData* equipData = GetEquipDataByFormID(weapForm->formID);
	if (equipData && equipData->equippedData)
		loadedAmmoCount = static_cast<UInt32>(equipData->equippedData->unk18);

	// 충전식 장전의 경우 현재 장전된 탄환이 전부 소모됨
	if (shotCount > 0 && ammoType & AmmoType::kAmmoType_Changing)
		shotCount = loadedAmmoCount;

	// 발사될 탄환까지 고려하여 최종적으로 장전되어있는 탄환의 수를 계산함
	UInt32 calculatedLoadedAmmoCount = loadedAmmoCount > shotCount ? loadedAmmoCount - shotCount : 0;

	// 탄약 타입이 끝없는 & (충전식 장전 | 퓨전코어)인 경우 체크 무시
	if (ammoType & AmmoType::kAmmoType_NeverEnding && ammoType & (AmmoType::kAmmoType_Changing | AmmoType::kAmmoType_FusionCore))
		return;

	// 강제로 부족한 탄약 채울 필요가 없는 경우
	if (!forcedReplenishAmmo) {
		// 끝없는 전설이 아니고 현재 장전된 탄환수가 0이 아닌 경우 체크 무시
		if (ammoType ^ AmmoType::kAmmoType_NeverEnding && calculatedLoadedAmmoCount != 0) {
			return;
		}
		// 끝없는 전설일때 현재 장전된 탄환수가 [끝없는 전설 최소 탄약 * (최소탄약 배수 - 1)] 보다 클때 체크 무시
		else if (ammoType & AmmoType::kAmmoType_NeverEnding) {
			UInt32 minNeverEndingAmmoCheckLimit = uMinAmmoCapacityMult > 1 ? uNeverEndingCapacity * (uMinAmmoCapacityMult - 1) : 0;
			if (calculatedLoadedAmmoCount > minNeverEndingAmmoCheckLimit)
				return;
		}
	}

	// 현재 탄환의 총 보유수량 조회
	UInt32 totalAmmoCount = GetInventoryItemCount(*g_player, ammo);

	// 현재 무기의 장전가능한 탄환 수량 조회
	UInt32 ammoCapacity = 0;
	// 일반 무기의 경우 기존의 장전가능한 탄환 수량을 이용하면 됨
	if (ammoType == AmmoType::kAmmoType_Default || ammoType == AmmoType::kAmmoType_Changing)
		ammoCapacity = CurrentAmmoCapacity;
	// 끝없는 무기의 경우 끝없는 무기 기본 탄환 수량을 이용
	else if (ammoType & AmmoType::kAmmoType_NeverEnding)
		ammoCapacity = uNeverEndingCapacity;
	// 퓨전코어 무기의 경우 무조건 퓨전코어 1개
	else if (ammoType & AmmoType::kAmmoType_FusionCore)
		ammoCapacity = 1;

	// 7. 아이템 추가
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