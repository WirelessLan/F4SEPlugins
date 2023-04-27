#include "CACS.h"

#include <fstream>
#include <unordered_map>

#include <f4se/GameForms.h>

#include "Global.h"
#include "Utils.h"

namespace CACS {
	std::string rulePath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  "_Rules.txt" };

	std::unordered_map<std::uint32_t, std::string> actorRules;
	std::unordered_map<std::uint32_t, std::string> raceRules;

	std::unordered_map<std::uint32_t, std::unordered_map<std::string, std::string>> actorCustomPaths;
	std::unordered_map<std::uint32_t, std::unordered_map<std::string, std::string>> raceCustomPaths;

	std::uint8_t GetChar(const std::string& line, std::uint32_t& index) {
		if (index < line.length())
			return line[index++];
		return 0xFF;
	}

	std::string GetString(const std::string& line, std::uint32_t& index, char delimeter) {
		std::uint8_t ch;
		std::string retVal = "";

		while ((ch = GetChar(line, index)) != 0xFF) {
			if (ch == '#') {
				if (index > 0)
					index--;
				break;
			}

			if (delimeter != 0 && ch == delimeter)
				break;

			retVal += ch;
		}

		Utils::Trim(retVal);
		return retVal;
	}

	void ParseRules(std::fstream& ruleFile) {
		std::string ruleType, pluginName, formId, meshesPath, customNifPath;
		std::string line;

		while (std::getline(ruleFile, line)) {
			bool isCustomNifPath = false;
			std::uint32_t lineIdx = 0;

			Utils::Trim(line);
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

			meshesPath = Utils::ReplaceSlash(Utils::ToLower(meshesPath));

			if (lineIdx != line.length()) {
				isCustomNifPath = true;
				meshesPath = Utils::RemovePrefix(meshesPath, "meshes\\");

				customNifPath = GetString(line, lineIdx, 0);
				if (customNifPath.empty()) {
					_MESSAGE("[Warning] Cannot read the CustomNifPath: %s", line.c_str());
					continue;
				}

				customNifPath = Utils::RemovePrefix(Utils::ReplaceSlash(Utils::ToLower(customNifPath)), "meshes\\");
			}

			TESForm* ruleTargetForm = Utils::GetFormFromIdentifier(pluginName, formId);
			if (!ruleTargetForm) {
				_MESSAGE("[Warning] Cannot find the Form: %s", line.c_str());
				continue;
			}

			if (isCustomNifPath) {
				std::unordered_map<std::uint32_t, std::unordered_map<std::string, std::string>>* customPathMap;
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
					std::unordered_map<std::string, std::string> nMap;
					nMap.insert(std::make_pair(meshesPath, customNifPath));
					customPathMap->insert(std::make_pair(ruleTargetForm->formID, nMap));
				}
				else {
					it->second.insert(std::make_pair(meshesPath, customNifPath));
				}

				_MESSAGE("[Info] RuleType[%s] PluginName[%s] FormID[0x%08X] SrcPath[%s] DestPath[%s]", ruleType.c_str(), pluginName.c_str(), ruleTargetForm->formID, meshesPath.c_str(), customNifPath.c_str());
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

	bool CheckCACSRule(RuleType type, std::uint32_t formId) {
		if (formId == 0)
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

	const std::string GetCACSPath(RuleType type, std::uint32_t formId) {
		if (formId == 0)
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

	bool SetCustomPaths(RuleType type, std::uint32_t formId, const std::string& meshPath, const char* prefixPath, const char* subPath, std::string& o_prefixPath, std::string& o_subPath, std::string& o_customPath) {
		std::string prefixPathStr = Utils::ReplaceSlash(Utils::ToLower(prefixPath));
		std::string subPathStr = Utils::ReplaceSlash(Utils::ToLower(subPath));

		subPathStr = Utils::RemovePrefix(subPathStr, prefixPathStr);

		std::unordered_map<std::uint32_t, std::unordered_map<std::string, std::string>>* customPathMap;
		if (type == RuleType::kRuleType_Actor)
			customPathMap = &actorCustomPaths;
		else if (type == RuleType::kRuleType_Race)
			customPathMap = &raceCustomPaths;
		else
			return false;

		if (meshPath.empty())
			return false;

		// Find CustomNifPath
		auto upper_it = customPathMap->find(formId);
		if (upper_it != customPathMap->end()) {
			auto lower_it = upper_it->second.find(subPathStr);
			if (lower_it != upper_it->second.end()) {
				o_prefixPath = "meshes\\" + meshPath;
				o_subPath = lower_it->second;
				o_customPath = o_prefixPath + o_subPath;
				if (Utils::IsFileExists(o_customPath)) {
					Utils::Log("[Debug] Type[%s] FormID[0x%08X] : Path[%s] -> %s",
						type == RuleType::kRuleType_Actor ? "Actor" : "Race", formId, subPathStr.c_str(), o_customPath.c_str());
					return true;
				}

				o_prefixPath = "meshes\\";
				o_customPath = o_prefixPath + o_subPath;
				if (Utils::IsFileExists(o_customPath)) {
					Utils::Log("[Debug] Type[%s] FormID[0x%08X] : Path[%s] -> %s",
						type == RuleType::kRuleType_Actor ? "Actor" : "Race", formId, subPathStr.c_str(), o_customPath.c_str());
					return true;
				}
			}
		}

		o_prefixPath = "meshes\\" + meshPath;
		o_subPath = subPathStr;
		o_customPath = o_prefixPath + o_subPath;
		if (Utils::IsFileExists(o_customPath)) {
			Utils::Log("[Debug] Type[%s] FormID[0x%08X] : Path[%s] -> %s",
				type == RuleType::kRuleType_Actor ? "Actor" : "Race", formId, subPathStr.c_str(), o_customPath.c_str());
			return true;
		}

		Utils::Log("[Debug] Type[%s] FormID[0x%08X] : Path[%s] -> Set to default path",
			type == RuleType::kRuleType_Actor ? "Actor" : "Race", formId, subPathStr.c_str());
		return false;
	}
}