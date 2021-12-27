#include "Globals.h"

std::unordered_set<std::string> ActorSkeletonPaths;

// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

void HandleSkeletonPaths() {
	std::string skeletonTxtPath{ "Data\\F4SE\\Plugins\\" PLUGIN_NAME "_Skeletons.txt" };
	std::ifstream input_file(skeletonTxtPath);

	if (!input_file.is_open()) {
		_MESSAGE("%s_Skeletons.txt doesn't exist", PLUGIN_NAME);
		return;
	}

	std::string line;
	while (std::getline(input_file, line)) {
		trim(line);

		if (line.length() == 0)
			continue;

		if (line[0] == '#')
			continue;

		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
		_MESSAGE("Skeleton Path: %s", line.c_str());
		ActorSkeletonPaths.insert(line);
	}

	input_file.close();
}