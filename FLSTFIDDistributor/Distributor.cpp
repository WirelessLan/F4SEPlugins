#include "Global.h"

namespace Distributor {
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

		Utility::Trim(retVal);
		return retVal;
	}

	void LoadConfig(std::unordered_map<BGSListForm*, std::vector<TESForm*>>& map, const std::string& configPath) {
		std::ifstream configFile(configPath);
		if (!configFile.is_open()) {
			_MESSAGE("Cannot open the config file: %s", configPath.c_str());
			return;
		}

		std::string line;
		std::string listFormPlugin, listFormId, targetPlugin, targetFormId;
		while (std::getline(configFile, line)) {
			Utility::Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			UInt32 index = 0;

			listFormPlugin = GetNextData(line, index, '|');
			if (listFormPlugin.empty()) {
				_MESSAGE("Cannot read the FormID List's plugin name: %s", line.c_str());
				continue;
			}

			listFormId = GetNextData(line, index, '|');
			if (listFormId.empty()) {
				_MESSAGE("Cannot read the FormID List's formID: %s", line.c_str());
				continue;
			}

			targetPlugin = GetNextData(line, index, '|');
			if (targetPlugin.empty()) {
				_MESSAGE("Cannot read the target plugin name: %s", line.c_str());
				continue;
			}

			targetFormId = GetNextData(line, index, 0);
			if (targetFormId.empty()) {
				_MESSAGE("Cannot read the target formID: %s", line.c_str());
				continue;
			}

			TESForm* form = Utility::GetFormFromIdentifier(listFormPlugin, listFormId);
			if (!form) {
				_MESSAGE("Cannot find the FormID List Form: %s | %s", listFormPlugin.c_str(), listFormId.c_str());
				continue;
			}

			BGSListForm* listForm = DYNAMIC_CAST(form, TESForm, BGSListForm);
			if (!listForm) {
				_MESSAGE("Invalid FormID List Form!: %s | %s", listFormPlugin.c_str(), listFormId.c_str());
				continue;
			}

			form = Utility::GetFormFromIdentifier(targetPlugin, targetFormId);
			if (!form) {
				_MESSAGE("Cannot find the Form: %s | %s", targetPlugin.c_str(), targetFormId.c_str());
				continue;
			}

			auto it = map.find(listForm);
			if (it == map.end()) {
				std::vector<TESForm*> nVec{ form };
				map.insert(std::make_pair(listForm, nVec));
			}
			else {
				it->second.push_back(form);
			}
		}
	}

	std::unordered_map<BGSListForm*, std::vector<TESForm*>> LoadConfigs() {
		std::unordered_map<BGSListForm*, std::vector<TESForm*>> retMap;
		std::string configDir = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME);
		const char* match = "*.txt";

		for (IDirectoryIterator iter(configDir.c_str(), match); !iter.Done(); iter.Next()) {
			WIN32_FIND_DATA* fileData = iter.Get();
			std::string s_fileName = fileData->cFileName;

			LoadConfig(retMap, configDir + "\\" + s_fileName);
		}

		return retMap;
	}

	bool FormExists(TESForm* form, const tArray<TESForm*>& arr) {
		for (UInt32 ii = 0; ii < arr.count; ii++) {
			if (form == arr.entries[ii])
				return true;
		}

		return false;
	}

	void Distribute() {
		std::unordered_map<BGSListForm*, std::vector<TESForm*>> configs = LoadConfigs();

		for (std::pair<BGSListForm*, std::vector<TESForm*>> e : configs) {
			_MESSAGE("Distribute to FormID List %08X..", e.first->formID, e.first);
			for (TESForm* form : e.second) {
				if (!FormExists(form, e.first->forms)) {
					e.first->forms.Push(form);
					_MESSAGE("Add Form %08X", form->formID);
				}
			}
		}

	}
}