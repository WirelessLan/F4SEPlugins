#include "CACS.h"
#include "Globals.h"
#include "Utils.h"

#include "f4se/GameData.h"

#include <fstream>
#include <unordered_map>
#include <algorithm>

std::unordered_map<UInt32, std::string> actorRules;
std::unordered_map<UInt32, std::string> raceRules;

static void ParseRule(std::fstream& ruleFile) {
	std::string ruleType, pluginName, formIdStr, meshPath;
	std::string line;

	while (std::getline(ruleFile, line)) {
		UInt32 lineIdx = 0;
		UInt32 strLen = line.length();

		for (; lineIdx < strLen; lineIdx++) {
			if (!std::isspace(line[lineIdx]))
				break;
		}

		if (lineIdx == strLen || line[lineIdx] == '#')
			continue;

		ruleType = "";
		for (; lineIdx < strLen; lineIdx++) {
			if (line[lineIdx] == '|') {
				lineIdx++;
				break;
			}

			ruleType += line[lineIdx];
		}
		if (lineIdx == strLen) {
			_MESSAGE("Can't read ruleType!!!");
			break;
		}
		trim(ruleType);

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
			break;
		}
		trim(pluginName);

		formIdStr = "";
		for (; lineIdx < strLen; lineIdx++) {
			if (line[lineIdx] == ':') {
				lineIdx++;
				break;
			}
			if (std::isspace(line[lineIdx]))
				continue;
			formIdStr += line[lineIdx];
		}
		if (lineIdx == strLen) {
			_MESSAGE("Can't read formId!!!");
			break;
		}

		meshPath = "";
		for (; lineIdx < strLen; lineIdx++) {
			if (line[lineIdx] == '\n' || line[lineIdx] == '\0')
				break;
			meshPath += line[lineIdx];
		}
		trim(meshPath);
		if (meshPath[meshPath.length() - 1] != '\\')
			meshPath += '\\';

		const ModInfo* modInfo = (*g_dataHandler)->LookupModByName(pluginName.c_str());
		if (!modInfo) {
			_MESSAGE("Can't find plugin[%s]!!!", pluginName.c_str());
			continue;
		}

		UInt32 pluginIndex = modInfo->GetPartialIndex();
		if (!modInfo->IsLight())
			pluginIndex = pluginIndex << 24;
		else
			pluginIndex = pluginIndex << 12;

		UInt32 formId = pluginIndex + strtoul(formIdStr.c_str(), nullptr, 16);
		_MESSAGE("ruleType[%s] pluginName[%s] formId[0x%08X] meshPath[%s]", ruleType.c_str(), pluginName.c_str(), formId, meshPath.c_str());

		if (_stricmp(ruleType.c_str(), "Actor") == 0)
			actorRules.insert(std::pair<UInt32, std::string>(formId, meshPath));
		else if (_stricmp(ruleType.c_str(), "Race") == 0)
			raceRules.insert(std::pair<UInt32, std::string>(formId, meshPath));
		else
			_MESSAGE("Unknown ruleType[%s]... skip...", ruleType.c_str());
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
	if (type == ActorType) {
		auto it = actorRules.find(formId);
		if (it == actorRules.end())
			return "";
		return it->second;
	}
	else if (type == RaceType) {
		auto it = raceRules.find(formId);
		if (it == raceRules.end())
			return "";
		return it->second;
	}
	else
		return "";
}