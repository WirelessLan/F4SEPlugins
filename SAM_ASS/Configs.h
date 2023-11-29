#pragma once

#include <unordered_set>

namespace Configs {
	extern std::unordered_set<std::string> g_actorSkeletonPaths;
	void LoadConfig();
};