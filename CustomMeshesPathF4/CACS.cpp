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

	std::uint8_t GetChar(const std::string& a_line, std::uint32_t& a_index) {
		if (a_index < a_line.length())
			return a_line[a_index++];
		return 0xFF;
	}

	std::string GetString(const std::string& a_line, std::uint32_t& a_index, char a_delimeter) {
		std::uint8_t ch;
		std::string retVal = "";

		while ((ch = GetChar(a_line, a_index)) != 0xFF) {
			if (ch == '#') {
				if (a_index > 0)
					a_index--;
				break;
			}

			if (a_delimeter != 0 && ch == a_delimeter)
				break;

			retVal += ch;
		}

		Utils::Trim(retVal);
		return retVal;
	}

	void ParseRules(std::fstream& a_ruleFile) {
		std::string ruleType, pluginName, formId, meshesPath, customNifPath;
		std::string line;

		while (std::getline(a_ruleFile, line)) {
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

	bool CheckCACSRule(RuleType a_type, std::uint32_t a_formId) {
		if (a_formId == 0)
			return false;

		switch (a_type) {
		case RuleType::kRuleType_Actor:
			if (actorRules.find(a_formId) != actorRules.end())
				return true;
			break;

		case RuleType::kRuleType_Race:
			if (raceRules.find(a_formId) != raceRules.end())
				return true;
			break;
		}

		return false;
	}

	const std::string GetCACSPath(RuleType a_type, std::uint32_t a_formId) {
		if (a_formId == 0)
			return std::string();

		switch (a_type) {
		case RuleType::kRuleType_Actor: {
			auto it = actorRules.find(a_formId);
			if (it != actorRules.end())
				return it->second;
			break;
		}
		case RuleType::kRuleType_Race: {
			auto it = raceRules.find(a_formId);
			if (it != raceRules.end())
				return it->second;
			break;
		}
		}

		return std::string();
	}

	bool SetCustomPaths(RuleType a_type, std::uint32_t a_formId, const std::string& a_customPath, const char* a_prefixPath, const char* a_subPath, std::string& o_prefixPath, std::string& o_subPath, std::string& o_customPath) {
		std::string prefixPathStr = Utils::ReplaceSlash(Utils::ToLower(a_prefixPath));
		std::string subPathStr = Utils::RemovePrefix(Utils::RemovePrefix(Utils::ReplaceSlash(Utils::ToLower(a_subPath)), "data\\"), prefixPathStr);

		subPathStr = Utils::RemovePrefix(subPathStr, prefixPathStr);

		std::unordered_map<std::uint32_t, std::unordered_map<std::string, std::string>>* customPathMap;
		if (a_type == RuleType::kRuleType_Actor)
			customPathMap = &actorCustomPaths;
		else if (a_type == RuleType::kRuleType_Race)
			customPathMap = &raceCustomPaths;
		else
			return false;

		// Find CustomPath
		auto upper_it = customPathMap->find(a_formId);
		if (upper_it != customPathMap->end()) {
			auto lower_it = upper_it->second.find(subPathStr);
			if (lower_it != upper_it->second.end()) {
				o_prefixPath = prefixPathStr + a_customPath;
				o_subPath = lower_it->second;
				o_customPath = o_prefixPath + o_subPath;
				if (Utils::IsFileExists(o_customPath)) {
					Utils::Log("[Debug] Type[%s] FormID[0x%08X] : Prefix[%s] Path[%s] -> %s",
						a_type == RuleType::kRuleType_Actor ? "Actor" : "Race", a_formId, prefixPathStr.c_str(), subPathStr.c_str(), o_customPath.c_str());
					return true;
				}

				o_prefixPath = prefixPathStr;
				o_customPath = o_prefixPath + o_subPath;
				if (Utils::IsFileExists(o_customPath)) {
					Utils::Log("[Debug] Type[%s] FormID[0x%08X] : Prefix[%s] Path[%s] -> %s",
						a_type == RuleType::kRuleType_Actor ? "Actor" : "Race", a_formId, prefixPathStr.c_str(), subPathStr.c_str(), o_customPath.c_str());
					return true;
				}
			}
		}

		o_prefixPath = prefixPathStr + a_customPath;
		o_subPath = subPathStr;
		o_customPath = o_prefixPath + o_subPath;
		if (Utils::IsFileExists(o_customPath)) {
			Utils::Log("[Debug] Type[%s] FormID[0x%08X] : Prefix[%s] Path[%s] -> %s",
				a_type == RuleType::kRuleType_Actor ? "Actor" : "Race", a_formId, prefixPathStr.c_str(), subPathStr.c_str(), o_customPath.c_str());
			return true;
		}

		Utils::Log("[Debug] Type[%s] FormID[0x%08X] : Prefix[%s] Path[%s] -> Set to default path",
			a_type == RuleType::kRuleType_Actor ? "Actor" : "Race", a_formId, prefixPathStr.c_str(), subPathStr.c_str());
		return false;
	}
}