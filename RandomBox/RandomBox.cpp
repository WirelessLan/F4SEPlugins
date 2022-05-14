#include "Global.h"

std::unordered_map<UInt32, std::vector<TESLeveledList::Entry>> g_leveledListEntries;
const std::string settingFilePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  ".txt" };
time_t settingFileLoadedTime = 0;

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

bool ReadSettingFile() {
	std::fstream settingFile(settingFilePath);
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
			_MESSAGE("Cannot read llFormIdStr! [%s]", line.c_str());
			continue;
		}

		pluginName = GetNextData(line, index, '|');
		if (pluginName.empty()) {
			_MESSAGE("Cannot read pluginName! [%s]", line.c_str());
			continue;
		}

		formId = GetNextData(line, index, 0);
		if (formId.empty()) {
			_MESSAGE("Cannot read formId! [%s]", line.c_str());
			continue;
		}

		TESForm* weapForm = GetFormFromIdentifier(pluginName, formId);
		if (!weapForm) {
			_MESSAGE("Cannot find Weapon! [%s | %s]", pluginName.c_str(), formId.c_str());
			continue;
		}

		UInt32 llFormId = std::stoul(llFormIdStr, nullptr, 16) & 0xFFFFFF;
		auto it = g_leveledListEntries.find(llFormId);
		if (it == g_leveledListEntries.end()) {
			std::vector<TESLeveledList::Entry> nEntry = { { weapForm, nullptr, 1, 1, 0 } };
			g_leveledListEntries.insert(std::make_pair(llFormId, nEntry));
		}
		else {
			it->second.push_back({ weapForm, nullptr, 1, 1, 0 });
		}
	}

	settingFile.close();

	return true;
}

void SetLeveledList(const UInt32 formId, const std::vector<TESLeveledList::Entry>& lle) {
	TESForm* ll_form = GetFormFromIdentifier("RandomBox.esp", formId);
	if (!ll_form)
		return;

	TESLevItem* ll_randBox = DYNAMIC_CAST(ll_form, TESForm, TESLevItem);
	if (!ll_randBox)
		return;

	if (ll_randBox->leveledList.entries) {
		Heap_Free(ll_randBox->leveledList.entries);
		ll_randBox->leveledList.entries = nullptr;
		ll_randBox->leveledList.length = 0;
	}

	if (lle.size() == 0)
		return;

	size_t entrySize = sizeof(TESLeveledList::Entry) * lle.size();
	ll_randBox->leveledList.entries = (TESLeveledList::Entry*)Heap_Allocate(entrySize);
	if (!ll_randBox->leveledList.entries)
		return;

	memcpy(ll_randBox->leveledList.entries, lle.data(), entrySize);
	ll_randBox->leveledList.length = lle.size();
}

bool ShouldReadSettingFile() {
	struct _stat64 stat;
	if (_stat64(settingFilePath.c_str(), &stat) != 0)
		return false;

	if (settingFileLoadedTime == 0 || settingFileLoadedTime != stat.st_mtime) {
		settingFileLoadedTime = stat.st_mtime;
		return true;
	}

	return false;
}

void Install_RandomBox() {
	if (!ShouldReadSettingFile())
		return;

	g_leveledListEntries.clear();

	if (!ReadSettingFile()) {
		_MESSAGE("Cannot read RandomBox setting file!");
		_MESSAGE("RandomBox will not work");
		return;
	}

	for (auto& it : g_leveledListEntries)
		SetLeveledList(it.first, it.second);

	_MESSAGE("RandomBox Installed!");
}