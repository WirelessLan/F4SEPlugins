#include "Configs.h"

#include <fstream>

#include "Utils.h"
#include "Version.h"

namespace Configs {
	std::unordered_set<std::string> g_actorSkeletonPaths;
	std::unordered_map<std::string, std::string> g_nodeMap;

	void LoadSkeletons() {
		const char* path = "Data\\F4SE\\Plugins\\" PLUGIN_NAME "_Skeletons.txt";
		std::ifstream input_file(path);
		if (!input_file.is_open()) {
			_MESSAGE("[Error] Failed to open the config file: %s", path);
			return;
		}

		std::string line;
		while (std::getline(input_file, line)) {
			line = Utils::Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			std::string skeletonPath = Utils::NormalizePath(Utils::ToLower(line));
			_MESSAGE("[Info] Skeleton Path: %s", skeletonPath.c_str());
			g_actorSkeletonPaths.insert(skeletonPath);
		}
	}

	void LoadNodes() {
		const char* path = "Data\\F4SE\\Plugins\\" PLUGIN_NAME "_Nodes.txt";
		std::ifstream input_file(path);
		if (!input_file.is_open()) {
			_MESSAGE("[Error] Failed to open the config file: %s", path);
			return;
		}

		std::string line;
		while (std::getline(input_file, line)) {
			line = Utils::Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			auto delimiter = line.find('|');
			if (delimiter == std::string::npos) {
				_MESSAGE("[Error] Invalid node data: %s", line.c_str());
				continue;
			}

			std::string baseNode = Utils::Trim(line.substr(0, delimiter));
			std::string samNode = Utils::Trim(line.substr(delimiter + 1));

			_MESSAGE("[Info] Node: %s | %s", baseNode.c_str(), samNode.c_str());
			g_nodeMap.insert(std::make_pair(baseNode, samNode));
		}
	}

	void LoadConfig() {
		LoadSkeletons();
		LoadNodes();
	}
}