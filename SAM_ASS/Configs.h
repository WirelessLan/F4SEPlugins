#pragma once

#include <unordered_map>
#include <unordered_set>

namespace Configs {
	extern std::unordered_set<std::string> g_actorSkeletonPaths;
	extern std::unordered_map<std::string, std::string> g_nodeMap;
	void LoadConfig();
};