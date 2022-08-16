#include "Globals.h"

std::unordered_set<std::string> ActorSkeletonPaths;

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
		if (line.empty() || line[0] == '#')
			continue;

		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
		_MESSAGE("Skeleton Path: %s", line.c_str());
		ActorSkeletonPaths.insert(line);
	}
}

char GetChar(const std::string& line, UInt32& index) {
	if (index < line.length())
		return line[index++];

	return -1;
}

std::string GetString(const std::string& line, UInt32& index, char delimeter) {
	char ch;
	std::string retVal = "";

	while ((ch = GetChar(line, index)) > 0) {
		if (ch == '#') {
			if (index > 0)
				index--;
			break;
		}

		if (delimeter != 0 && ch == delimeter)
			break;

		retVal += ch;
	}

	trim(retVal);
	return retVal;
}

void ParseFile(WeaponPosition& targetPos, std::fstream& file) {
	std::string line;

	while (std::getline(file, line)) {
		UInt32 lineIdx = 0;

		trim(line);
		if (line.empty() || line[0] == '#')
			continue;

		std::string name = GetString(line, lineIdx, ':');
		if (name.empty()) {
			_MESSAGE("Cannot read a property name: %s", line.c_str());
			continue;
		}

		std::string value = GetString(line, lineIdx, 0);
		if (value.empty()) {
			_MESSAGE("Cannot read a property value: %s", line.c_str());
			continue;
		}

		if (_stricmp(name.c_str(), "eulerHeading") == 0)
			targetPos.eulerHeading = std::stod(value);
		else if (_stricmp(name.c_str(), "eulerAttitude") == 0)
			targetPos.eulerAttitude = std::stod(value);
		else if (_stricmp(name.c_str(), "eulerBank") == 0)
			targetPos.eulerBank = std::stod(value);
		else if (_stricmp(name.c_str(), "posX") == 0)
			targetPos.posX = std::stod(value);
		else if (_stricmp(name.c_str(), "posY") == 0)
			targetPos.posY = std::stod(value);
		else if (_stricmp(name.c_str(), "posZ") == 0)
			targetPos.posZ = std::stod(value);
		else if (_stricmp(name.c_str(), "potentialParentNode") == 0)
			targetPos.potentialParentNode = value;
		else
			_MESSAGE("Unknown property name: %s", name.c_str());
	}
}

void LoadPosVec(WeaponPosition& targetPos) {
	std::string posVecPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "_" + targetPos.name + ".txt";
	std::fstream posVecFile(posVecPath);
	if (!posVecFile.is_open()) {
		_MESSAGE("Cannot open a PosVec file: %s", posVecPath.c_str());
		return;
	}

	ParseFile(targetPos, posVecFile);
}