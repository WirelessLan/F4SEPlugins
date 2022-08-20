#include "Global.h"

namespace ConfigReader {
	std::string configPath = { "Data\\F4SE\\Plugins\\" PLUGIN_NAME ".cfg" };
	time_t configLoadedTime = 0;
	CaseInsensitiveMap<time_t> pluginPoseConfigLoadedTime;

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

	// Get Plugin List
	std::vector<std::string> ReadConfig() {
		std::ifstream configFile(configPath);
		std::vector<std::string> retVec;

		if (!configFile.is_open()) {
			_MESSAGE("Cannot open a config file: %s", configPath.c_str());
			return retVec;
		}

		std::string line;
		std::string pluginName;
		while (std::getline(configFile, line)) {
			Utility::Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			UInt32 index = 0;

			pluginName = GetNextData(line, index, 0);
			if (pluginName.empty()) {
				_MESSAGE("Cannot read Plugin Name: %s", line.c_str());
				continue;
			}

			retVec.push_back(pluginName);
		}

		return retVec;
	}

	std::vector<std::string> ReadPluginPoseConfig(const std::string& pluginName) {
		std::string poseConfigPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\" + pluginName + ".cfg";
		std::ifstream configFile(poseConfigPath);
		std::vector<std::string> retVec;

		if (!configFile.is_open())
			return retVec;

		std::string line;
		std::string poseName;
		while (std::getline(configFile, line)) {
			Utility::Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			UInt32 index = 0;

			poseName = GetNextData(line, index, 0);
			if (poseName.empty()) {
				_MESSAGE("Cannot read Pose Name: %s", line.c_str());
				continue;
			}

			retVec.push_back(poseName);
		}

		return retVec;
	}

	bool ShouldReadConfig() {
		struct _stat64 stat;
		if (_stat64(configPath.c_str(), &stat) != 0)
			return false;

		if (configLoadedTime == 0 || configLoadedTime != stat.st_mtime) {
			configLoadedTime = stat.st_mtime;
			return true;
		}

		return false;
	}

	bool ShouldReadPluginPoseConfig(const std::string& pluginName) {
		std::string poseConfigPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\" + pluginName + ".cfg";
		struct _stat64 stat;
		if (_stat64(poseConfigPath.c_str(), &stat) != 0) {
			pluginPoseConfigLoadedTime.erase(pluginName);
			return false;
		}

		auto it = pluginPoseConfigLoadedTime.find(pluginName);
		if (it == pluginPoseConfigLoadedTime.end()) {
			pluginPoseConfigLoadedTime.insert(std::make_pair(pluginName, stat.st_mtime));
			return true;
		}
		else if (it->second != stat.st_mtime) {
			pluginPoseConfigLoadedTime[pluginName] = stat.st_mtime;
			return true;
		}

		return false;
	}
}