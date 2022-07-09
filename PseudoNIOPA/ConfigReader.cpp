#include "Global.h"

namespace ConfigReader {
	std::string GetConfigOption(const char* section, const char* key) {
		char	result[256] = { 0 };
		std::string configPath{ "Data\\F4SE\\Plugins\\" PLUGIN_NAME ".ini" };

		GetPrivateProfileString(section, key, NULL, result, sizeof(result), configPath.c_str());
		return result;
	}

	bool GetConfigValue(const char* section, const char* key, std::string* dataOut) {
		std::string	data = GetConfigOption(section, key);
		if (data.empty())
			return false;

		*dataOut = data;
		return true;
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

		Utility::Trim(retVal);
		return retVal;
	}

	std::vector<std::string> GetNodeListFromFile(std::string& groupName) {
		std::string configPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\" + groupName + ".txt";
		std::ifstream configFile(configPath);
		std::vector<std::string> retVec;

		if (!configFile.is_open()) {
			_MESSAGE("Cannot open a config file! [%s]", configPath.c_str());
			return retVec;
		}

		std::string line;
		std::string nodeName;
		while (std::getline(configFile, line)) {
			Utility::Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			UInt32 index = 0;

			nodeName = GetNextData(line, index, 0);
			if (nodeName.empty()) {
				_MESSAGE("Cannot read a Node Name[%s]", line.c_str());
				continue;
			}

			retVec.push_back(nodeName);
		}

		return retVec;
	}
}