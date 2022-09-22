#include "Global.h"

std::string rulePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  "_Rules.txt" };

std::unordered_map<UInt32, std::string> actorRules;
std::unordered_map<UInt32, std::string> raceRules;

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
	std::string ruleType, pluginName, formId, meshesPath;
	std::string line;

	while (std::getline(ruleFile, line)) {
		UInt32 lineIdx = 0;

		trim(line);
		if (line.empty() || line[0] == '#')
			continue;

		ruleType = GetString(line, lineIdx, '|');
		if (ruleType.empty()) {
			_MESSAGE("Cannot read the ruleType - %s", line.c_str());
			continue;
		}

		pluginName = GetString(line, lineIdx, '|');
		if (pluginName.empty()) {
			_MESSAGE("Cannot read the pluginName - %s", line.c_str());
			continue;
		}

		formId = GetString(line, lineIdx, ':');
		if (formId.empty()) {
			_MESSAGE("Cannot read the formId - %s", line.c_str());
			continue;
		}

		meshesPath = GetString(line, lineIdx, 0);
		if (meshesPath.empty()) {
			_MESSAGE("Cannot read the meshesPath - %s", line.c_str());
			continue;
		}

		if (meshesPath[meshesPath.length() - 1] != '\\')
			meshesPath += '\\';

		TESForm* ruleTargetForm = GetFormFromIdentifier(pluginName, formId);
		if (!ruleTargetForm) {
			_MESSAGE("Cannot find the Form - %s", line.c_str());
			continue;
		}

		if (_stricmp(ruleType.c_str(), "Actor") == 0)
			actorRules.insert(std::pair<UInt32, std::string>(ruleTargetForm->formID, meshesPath));
		else if (_stricmp(ruleType.c_str(), "Race") == 0)
			raceRules.insert(std::pair<UInt32, std::string>(ruleTargetForm->formID, meshesPath));
		else {
			_MESSAGE("Unknown ruleType - %s", line.c_str());
			continue;
		}

		_MESSAGE("ruleType[%s] pluginName[%s] formId[0x%08X] meshesPath[%s]", ruleType.c_str(), pluginName.c_str(), ruleTargetForm->formID, meshesPath.c_str());
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
		_MESSAGE("No Rules file found");
		return;
	}

	actorRules.clear();
	raceRules.clear();

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