#pragma once
namespace ConfigReader {
	bool GetConfigValue(const char* section, const char* key, std::string* dataOut);
	std::string GetNodeNameFromFile(std::string& groupName, UInt32 selection);
};