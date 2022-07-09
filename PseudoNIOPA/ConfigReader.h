#pragma once
namespace ConfigReader {
	bool GetConfigValue(const char* section, const char* key, std::string* dataOut);
	std::vector<std::string> GetNodeListFromFile(std::string& groupName);
};