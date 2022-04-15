#include "Global.h"

std::string rulePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  "_Rules.txt" };
time_t ruleLoadedTime = 0;

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
			_MESSAGE("Can't read ruleType!!!");
			continue;
		}

		pluginName = GetString(line, lineIdx, '|');
		if (pluginName.empty()) {
			_MESSAGE("Can't read pluginName!!!");
			continue;
		}

		formId = GetString(line, lineIdx, ':');
		if (formId.empty()) {
			_MESSAGE("Can't read formId!!!");
			continue;
		}

		meshesPath = GetString(line, lineIdx, 0);
		if (meshesPath.empty()) {
			_MESSAGE("Can't read meshesPath!!!");
			continue;
		}

		if (meshesPath[meshesPath.length() - 1] != '\\')
			meshesPath += '\\';

		TESForm* ruleTargetForm = GetFormFromIdentifier(pluginName, formId);
		if (!ruleTargetForm) {
			_MESSAGE("Can't find Form[%s|%s]!!!", pluginName.c_str(), formId.c_str());
			continue;
		}

		if (_stricmp(ruleType.c_str(), "Actor") == 0)
			actorRules.insert(std::pair<UInt32, std::string>(ruleTargetForm->formID, meshesPath));
		else if (_stricmp(ruleType.c_str(), "Race") == 0)
			raceRules.insert(std::pair<UInt32, std::string>(ruleTargetForm->formID, meshesPath));
		else
			_MESSAGE("Unknown ruleType[%s]...", ruleType.c_str());

		_MESSAGE("ruleType[%s] pluginName[%s] formId[0x%08X] meshesPath[%s]", ruleType.c_str(), pluginName.c_str(), ruleTargetForm->formID, meshesPath.c_str());
	}
}

bool ShouldLoadRules() {
	struct _stat64 stat;
	if (_stat64(rulePath.c_str(), &stat) != 0)
		return false;

	if (ruleLoadedTime == 0 || ruleLoadedTime != stat.st_mtime) {
		ruleLoadedTime = stat.st_mtime;
		return true;
	}

	return false;
}

void LoadRules() {
	std::fstream ruleFile(rulePath);
	if (!ruleFile.is_open()) {
		_MESSAGE("Cannot open a Rules file!!");
		return;
	}

	actorRules.clear();
	raceRules.clear();

	ParseRules(ruleFile);
	ruleFile.close();
}

bool CheckCACSRule(UInt32 raceId, UInt32 actorId) {
	if (raceRules.empty() && actorRules.empty())
		return false;

	auto race_it = raceRules.find(raceId);
	auto actor_it = actorRules.find(actorId);
	if (race_it == raceRules.end() && actor_it == actorRules.end())
		return false;

	return true;
}

const std::string GetCACSPath(RuleType type, UInt32 formId) {
	if (type == RuleType::kRuleType_Actor) {
		auto it = actorRules.find(formId);
		if (it == actorRules.end())
			return "";
		return it->second;
	}
	else if (type == RuleType::kRuleType_Race) {
		auto it = raceRules.find(formId);
		if (it == raceRules.end())
			return "";
		return it->second;
	}
	else
		return "";
}