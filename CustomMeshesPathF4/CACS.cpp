#include "Global.h"

std::unordered_map<UInt32, std::string> actorRules;
std::unordered_map<UInt32, std::string> raceRules;

std::string GetString(const std::string& line, UInt32& index, char delimeter) {
	std::string retVal = "";
	for (; index < line.length(); index++) {
		if (delimeter != 0 && line[index] == delimeter) {
			index++;
			break;
		}

		retVal += line[index];
	}

	return retVal;
}

void ParseRule(std::fstream& ruleFile) {
	std::string ruleType, pluginName, formId, meshesPath;
	std::string line;

	while (std::getline(ruleFile, line)) {
		trim(line);
		if (line.length() == 0 || line[0] == '#')
			continue;

		UInt32 lineIdx = 0;

		ruleType = GetString(line, lineIdx, '|');
		trim(ruleType);
		if (ruleType == "") {
			_MESSAGE("Can't read ruleType!!!");
			continue;
		}

		pluginName = GetString(line, lineIdx, '|');
		trim(pluginName);
		if (pluginName == "") {
			_MESSAGE("Can't read pluginName!!!");
			continue;
		}

		formId = GetString(line, lineIdx, ':');
		trim(formId);
		if (formId == "") {
			_MESSAGE("Can't read formId!!!");
			continue;
		}

		meshesPath = GetString(line, lineIdx, 0);
		trim(meshesPath);
		if (meshesPath == "") {
			_MESSAGE("Can't read meshesPath!!!");
			continue;
		}

		if (meshesPath[meshesPath.length() - 1] != '\\')
			meshesPath += '\\';

		TESForm* ruleTargetForm = GetFormFromIdentifier(pluginName + "|" + formId);
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

void InitCACS() {
	std::string rulePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  "_Rules.txt" };

	std::fstream ruleFile(rulePath);
	if (!ruleFile.is_open()) {
		_MESSAGE("Can't open a rule file!!");
		return;
	}

	ParseRule(ruleFile);
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