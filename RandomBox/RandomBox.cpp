#include "Global.h"
#include <fstream>
#include <vector>

std::vector<TESLeveledList::Entry> lle_HandGuns;
std::vector<TESLeveledList::Entry> lle_SniperRifles;
std::vector<TESLeveledList::Entry> lle_SMGs;
std::vector<TESLeveledList::Entry> lle_Shotguns;
std::vector<TESLeveledList::Entry> lle_AssaultRifles;
std::vector<TESLeveledList::Entry> lle_DMRs;
std::vector<TESLeveledList::Entry> lle_Specials;
std::vector<TESLeveledList::Entry> lle_SVD;
std::vector<TESLeveledList::Entry> lle_ASVal;
std::vector<TESLeveledList::Entry> lle_AK74M;

bool Read_SettingFile() {
	std::string settingFilePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  ".txt" };
	std::fstream settingFile(settingFilePath);
	if (!settingFile.is_open())
		return false;

	std::string line;
	std::string weapType, pluginName, formId;
	while (std::getline(settingFile, line)) {
		UInt32 lineIdx = 0;
		UInt32 strLen = line.length();

		for (; lineIdx < strLen; lineIdx++) {
			if (!std::isspace(line[lineIdx]))
				break;
		}

		if (lineIdx == strLen || line[lineIdx] == '#')
			continue;

		weapType = "";
		for (; lineIdx < strLen; lineIdx++) {
			if (line[lineIdx] == '|') {
				lineIdx++;
				break;
			}

			weapType += line[lineIdx];
		}
		if (lineIdx == strLen) {
			_MESSAGE("Can't read weapType!!!");
			continue;
		}
		trim(weapType);

		pluginName = "";
		for (; lineIdx < strLen; lineIdx++) {
			if (line[lineIdx] == '|') {
				lineIdx++;
				break;
			}

			pluginName += line[lineIdx];
		}
		if (lineIdx == strLen) {
			_MESSAGE("Can't read pluginName!!!");
			continue;
		}
		trim(pluginName);

		formId = "";
		for (; lineIdx < strLen; lineIdx++)
			formId += line[lineIdx];
		if (formId.length() == 0) {
			_MESSAGE("Can't read formId!!!");
			continue;
		}
		trim(formId);

		std::string weapFormId = pluginName + "|" + formId;
		TESForm* weapForm = GetFormFromIdentifier(weapFormId.c_str());
		if (!weapForm) {
			_MESSAGE("Can't find Weapon! %s | %s", pluginName.c_str(), formId.c_str());
			continue;
		}

		_MESSAGE("weapType[%s] pluginName[%s] formId[%s]", weapType.c_str(), pluginName.c_str(), formId.c_str());

		if (_stricmp(weapType.c_str(), "HandGun") == 0)
			lle_HandGuns.push_back({ weapForm, nullptr, 1, 1, 0 });
		else if (_stricmp(weapType.c_str(), "SniperRifle") == 0)
			lle_SniperRifles.push_back({ weapForm, nullptr, 1, 1, 0 });
		else if (_stricmp(weapType.c_str(), "SMG") == 0)
			lle_SMGs.push_back({ weapForm, nullptr, 1, 1, 0 });
		else if (_stricmp(weapType.c_str(), "Shotgun") == 0)
			lle_Shotguns.push_back({ weapForm, nullptr, 1, 1, 0 });
		else if (_stricmp(weapType.c_str(), "AssaultRifle") == 0)
			lle_AssaultRifles.push_back({ weapForm, nullptr, 1, 1, 0 });
		else if (_stricmp(weapType.c_str(), "DMR") == 0)
			lle_DMRs.push_back({ weapForm, nullptr, 1, 1, 0 });
		else if (_stricmp(weapType.c_str(), "Special") == 0)
			lle_Specials.push_back({ weapForm, nullptr, 1, 1, 0 });
		else if (_stricmp(weapType.c_str(), "SVD") == 0)
			lle_SVD.push_back({ weapForm, nullptr, 1, 1, 0 });
		else if (_stricmp(weapType.c_str(), "ASVal") == 0)
			lle_ASVal.push_back({ weapForm, nullptr, 1, 1, 0 });
		else if (_stricmp(weapType.c_str(), "AK74M") == 0)
			lle_AK74M.push_back({ weapForm, nullptr, 1, 1, 0 });
		else
			_MESSAGE("Unknown weapType: %s", weapType.c_str());
	}

	return true;
}

void SetLeveledList(const std::string& formId, const std::vector<TESLeveledList::Entry>& lle) {
	TESForm* ll_form = GetFormFromIdentifier(formId);
	if (!ll_form)
		return;

	TESLevItem* ll_randBox = DYNAMIC_CAST(ll_form, TESForm, TESLevItem);
	if (!ll_randBox)
		return;

	ll_randBox->leveledList.entries = (TESLeveledList::Entry*)lle.data();
	ll_randBox->leveledList.length = lle.size();
}

void Install_RandomBox() {
	if (!Read_SettingFile()) {
		_MESSAGE("Can't read RandomBox setting file!");
		_MESSAGE("RandomBox will not work");
		return;
	}

	SetLeveledList("RandomBox.esp|814", lle_HandGuns);
	SetLeveledList("RandomBox.esp|81C", lle_SniperRifles);
	SetLeveledList("RandomBox.esp|81D", lle_SMGs);
	SetLeveledList("RandomBox.esp|81E", lle_Shotguns);
	SetLeveledList("RandomBox.esp|81F", lle_AssaultRifles);
	SetLeveledList("RandomBox.esp|83D", lle_DMRs);
	SetLeveledList("RandomBox.esp|83E", lle_Specials);
	SetLeveledList("RandomBox.esp|861", lle_SVD);
	SetLeveledList("RandomBox.esp|862", lle_ASVal);
	SetLeveledList("RandomBox.esp|863", lle_AK74M);

	_MESSAGE("RandomBox Installed!");
}