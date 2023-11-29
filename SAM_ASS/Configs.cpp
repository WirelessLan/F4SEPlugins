#include "Configs.h"

#include <fstream>

#include "Utils.h"
#include "Version.h"

namespace Configs {
	std::unordered_set<std::string> g_actorSkeletonPaths;
	const char* g_skeletonsTxtPath = "Data\\F4SE\\Plugins\\" PLUGIN_NAME "_Skeletons.txt";

	void LoadConfig() {
		std::ifstream input_file(g_skeletonsTxtPath);
		if (!input_file.is_open()) {
			_MESSAGE("[Error] Failed to open the config file: %s", g_skeletonsTxtPath);
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
}