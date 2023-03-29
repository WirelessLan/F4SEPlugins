#include "Global.h"

std::string rulePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  "_Rules.txt" };

std::unordered_map<UInt32, std::string> actorRules;
std::unordered_map<UInt32, std::string> raceRules;

std::unordered_map<UInt32, CaseInsensitiveMap<std::string>> actorCustomPaths;
std::unordered_map<UInt32, CaseInsensitiveMap<std::string>> raceCustomPaths;

char GetChar(const std::string& line, UInt32& index) {
	if (index < line.length())
		return line[index++];

	return -1;
}

std::string GetString(const std::string& line, UInt32& index, char delimeter) {
	char ch;
	std::string retVal = "";

	while ((ch = GetChar(line, index)) > 0) {
		if (ch == '#') {
			if (index > 0)
				index--;
			break;
		}

		if (delimeter != 0 && ch == delimeter)
			break;

		retVal += ch;
	}

	trim(retVal);
	return retVal;
}

void ParseRules(std::fstream& ruleFile) {
	std::string ruleType, pluginName, formId, meshesPath, customPath;
	std::string line;

	while (std::getline(ruleFile, line)) {
		bool isCustomNifPath = false;
		UInt32 lineIdx = 0;

		trim(line);
		if (line.empty() || line[0] == '#')
			continue;

		ruleType = GetString(line, lineIdx, '|');
		if (ruleType.empty()) {
			_MESSAGE("[Warning] Cannot read the RuleType: %s", line.c_str());
			continue;
		}

		pluginName = GetString(line, lineIdx, '|');
		if (pluginName.empty()) {
			_MESSAGE("[Warning] Cannot read the PluginName: %s", line.c_str());
			continue;
		}

		formId = GetString(line, lineIdx, ':');
		if (formId.empty()) {
			_MESSAGE("[Warning] Cannot read the FormId: %s", line.c_str());
			continue;
		}

		meshesPath = GetString(line, lineIdx, ':');
		if (meshesPath.empty()) {
			_MESSAGE("[Warning] Cannot read the Path: %s", line.c_str());
			continue;
		}

		meshesPath = toLower(meshesPath);

		if (lineIdx != line.length()) {
			isCustomNifPath = true;
			meshesPath = remove_prefix("meshes\\", meshesPath);

			customPath = GetString(line, lineIdx, 0);
			if (customPath.empty()) {
				_MESSAGE("[Warning] Cannot read the CustomPath: %s", line.c_str());
				continue;
			}

			customPath = toLower(customPath);
			customPath = remove_prefix("meshes\\", customPath);
		}

		TESForm* ruleTargetForm = GetFormFromIdentifier(pluginName, formId);
		if (!ruleTargetForm) {
			_MESSAGE("[Warning] Cannot find the Form: %s", line.c_str());
			continue;
		}

		if (isCustomNifPath) {
			std::unordered_map<UInt32, CaseInsensitiveMap<std::string>>* customPathMap;
			if (_stricmp(ruleType.c_str(), "Actor") == 0)
				customPathMap = &actorCustomPaths;
			else if (_stricmp(ruleType.c_str(), "Race") == 0)
				customPathMap = &raceCustomPaths;
			else {
				_MESSAGE("[Warning] Unknown RuleType: %s", line.c_str());
				continue;
			}

			auto it = customPathMap->find(ruleTargetForm->formID);
			if (it == customPathMap->end()) {
				CaseInsensitiveMap<std::string> nMap;
				nMap.insert(std::make_pair(meshesPath, customPath));
				customPathMap->insert(std::make_pair(ruleTargetForm->formID, nMap));
			}
			else {
				it->second.insert(std::make_pair(meshesPath, customPath));
			}

			_MESSAGE("[Info] RuleType[%s] PluginName[%s] FormID[0x%08X] SrcPath[%s] DestPath[%s]", ruleType.c_str(), pluginName.c_str(), ruleTargetForm->formID, meshesPath.c_str(), customPath.c_str());
		}
		else {
			if (meshesPath[meshesPath.length() - 1] != '\\')
				meshesPath += '\\';

			if (_stricmp(ruleType.c_str(), "Actor") == 0)
				actorRules.insert(std::make_pair(ruleTargetForm->formID, meshesPath));
			else if (_stricmp(ruleType.c_str(), "Race") == 0)
				raceRules.insert(std::make_pair(ruleTargetForm->formID, meshesPath));
			else {
				_MESSAGE("[Warning] Unknown RuleType: %s", line.c_str());
				continue;
			}

			_MESSAGE("[Info] RuleType[%s] PluginName[%s] FormID[0x%08X] CustomPath[%s]", ruleType.c_str(), pluginName.c_str(), ruleTargetForm->formID, meshesPath.c_str());
		}
	}
}

bool ShouldLoadRules() {
	struct _stat64 stat;
	if (_stat64(rulePath.c_str(), &stat) != 0)
		return false;

	static time_t ruleLoadedTime = 0;
	if (ruleLoadedTime == 0 || ruleLoadedTime != stat.st_mtime) {
		ruleLoadedTime = stat.st_mtime;
		return true;
	}

	return false;
}

void LoadRules() {
	std::fstream ruleFile(rulePath);
	if (!ruleFile.is_open()) {
		_MESSAGE("[Warning] Cannot open the file: %s", rulePath.c_str());
		return;
	}

	actorRules.clear();
	raceRules.clear();

	actorCustomPaths.clear();
	raceCustomPaths.clear();

	ParseRules(ruleFile);
	ruleFile.close();
}

bool CheckCACSRule(RuleType type, UInt32 formId) {
	if (formId == 0xFFFFFFFF)
		return false;

	switch (type) {
	case RuleType::kRuleType_Actor:
		if (actorRules.find(formId) == actorRules.end())
			return false;
		break;

	case RuleType::kRuleType_Race:
		if (raceRules.find(formId) == raceRules.end())
			return false;
		break;

	default:
		return false;
	}

	return true;
}

const std::string GetCACSPath(RuleType type, UInt32 formId) {
	if (formId == 0xFFFFFFFF)
		return std::string();

	switch (type) {
	case RuleType::kRuleType_Actor: {
		auto it = actorRules.find(formId);
		if (it != actorRules.end())
			return it->second;
		break;
	}
	case RuleType::kRuleType_Race: {
		auto it = raceRules.find(formId);
		if (it != raceRules.end())
			return it->second;
		break;
	}
	}

	return std::string();
}

bool IsEqualPath(const std::string& path1, const std::string& path2) {
	std::string path1WoPrefix = remove_prefix("meshes\\", path1);
	std::string path2WoPrefix = remove_prefix("meshes\\", path2);
	return path1WoPrefix == path2WoPrefix;
}

bool SetCustomPaths(RuleType type, UInt32 cId, const std::string& cPath, const std::string& subPath, std::string& o_prefixPath, std::string& o_subPath, std::string& o_fullPath) {
	if (cPath.empty())
		return false;

	std::unordered_map<UInt32, CaseInsensitiveMap<std::string>>* customPathMap;
	if (type == RuleType::kRuleType_Actor)
		customPathMap = &actorCustomPaths;
	else
		customPathMap = &raceCustomPaths;

	auto upper_it = customPathMap->find(cId);
	if (upper_it != customPathMap->end()) {
		auto lower_it = upper_it->second.find(subPath);
		if (lower_it != upper_it->second.end()) {
			o_prefixPath = "meshes\\" + cPath;
			o_subPath = lower_it->second;
			o_fullPath = "data\\" + o_prefixPath + o_subPath;
			if (IsFileExists(o_fullPath)) {
				Log("[Debug] Type[%s] FormID[0x%08X] : Path[%s] -> %s", 
					type == RuleType::kRuleType_Actor ? "Actor" : "Race", cId, subPath.c_str(), o_fullPath.c_str());
				return true;
			}

			o_prefixPath = "meshes\\";
			o_fullPath = "data\\" + o_prefixPath + o_subPath;
			if (IsFileExists(o_fullPath)) {
				Log("[Debug] Type[%s] FormID[0x%08X] : Path[%s] -> %s", 
					type == RuleType::kRuleType_Actor ? "Actor" : "Race", cId, subPath.c_str(), o_fullPath.c_str());
				return true;
			}
		}
	}

	o_prefixPath = "meshes\\" + cPath;
	o_subPath = subPath;
	o_fullPath = "data\\" + o_prefixPath + o_subPath;
	if (IsFileExists(o_fullPath)) {
		Log("[Debug] Type[%s] FormID[0x%08X] : Path[%s] -> %s", 
			type == RuleType::kRuleType_Actor ? "Actor" : "Race", cId, subPath.c_str(), o_fullPath.c_str());
		return true;
	}

	Log("[Debug] Type[%s] FormID[0x%08X] : Path[%s] -> Set to default path", 
		type == RuleType::kRuleType_Actor ? "Actor" : "Race", cId, subPath.c_str());
	return false;
}