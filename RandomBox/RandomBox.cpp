#include "Global.h"

std::unordered_map<UInt32, std::vector<TESLeveledList::Entry>> g_randomBoxLLs;

std::vector<TESLeveledList::Entry>* GetLLById(UInt32 llFormId) {
	auto it = g_randomBoxLLs.find(llFormId);
	if (it == g_randomBoxLLs.end())
		return nullptr;

	return &it->second;
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

bool Read_SettingFile() {
	static std::string settingFilePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  ".txt" };
	std::fstream settingFile(settingFilePath);

	if (g_randomBoxLLs.size() > 0) {
		for (auto& it : g_randomBoxLLs)
			it.second.clear();

		g_randomBoxLLs.clear();
	}

	if (!settingFile.is_open())
		return false;

	std::string line;
	std::string llFormIdStr, pluginName, formId;
	while (std::getline(settingFile, line)) {
		trim(line);
		if (line.length() == 0 || line[0] == '#')
			continue;

		UInt32 index = 0;

		llFormIdStr = GetNextData(line, index, '|');
		if (llFormIdStr.empty()) {
			_MESSAGE("Can't read llFormId!!!");
			continue;
		}

		pluginName = GetNextData(line, index, '|');
		if (pluginName.empty()) {
			_MESSAGE("Can't read pluginName!!!");
			continue;
		}

		formId = GetNextData(line, index, 0);
		if (formId.empty()) {
			_MESSAGE("Can't read formId!!!");
			continue;
		}

		TESForm* weapForm = GetFormFromIdentifier(pluginName, formId);
		if (!weapForm) {
			_MESSAGE("Can't find Weapon! %s | %s", pluginName.c_str(), formId.c_str());
			continue;
		}

		_MESSAGE("llFormId[%s] pluginName[%s] formId[%s][0x%08X]", llFormIdStr.c_str(), pluginName.c_str(), formId.c_str(), weapForm->formID);

		UInt32 llFormId = std::stoul(llFormIdStr, nullptr, 16) & 0xFFFFFF;
		auto ll_vec = GetLLById(llFormId);
		if (!ll_vec) {
			std::vector<TESLeveledList::Entry> ll_items = { { weapForm, nullptr, 1, 1, 0 } };
			g_randomBoxLLs.insert(std::make_pair(llFormId, ll_items));
		}
		else {
			ll_vec->push_back({ weapForm, nullptr, 1, 1, 0 });
		}
	}

	settingFile.close();

	return true;
}

void SetLeveledList(const UInt32 formId, const std::vector<TESLeveledList::Entry>& lle) {
	TESForm* ll_form = GetFormFromIdentifier("RandomBox.esp", formId);
	if (!ll_form) {
		_MESSAGE("Cannot find Leveled List![0x%s]", formId);
		return;
	}

	TESLevItem* ll_randBox = DYNAMIC_CAST(ll_form, TESForm, TESLevItem);
	if (!ll_randBox) {
		_MESSAGE("Cannot find Leveled List![0x%s]", formId);
		return;
	}

	if (ll_randBox->leveledList.entries)
		Heap_Free(ll_randBox->leveledList.entries);

	ll_randBox->leveledList.entries = (TESLeveledList::Entry*)lle.data();
	ll_randBox->leveledList.length = lle.size();
}

void Install_RandomBox() {
	if (!Read_SettingFile()) {
		_MESSAGE("Can't read RandomBox setting file!");
		_MESSAGE("RandomBox will not work");
		return;
	}

	for each (auto& ll_elem in g_randomBoxLLs)
		SetLeveledList(ll_elem.first, ll_elem.second);

	_MESSAGE("RandomBox Installed!");
}