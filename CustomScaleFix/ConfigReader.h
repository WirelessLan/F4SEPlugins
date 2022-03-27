#pragma once

template<typename T>
bool GetConfigValue(const char* section, const char* key, T* dataOut);
template<>
bool GetConfigValue(const char* section, const char* key, bool* dataOut);
template<>
bool GetConfigValue(const char* section, const char* key, UInt32* dataOut);
template<>
bool GetConfigValue(const char* section, const char* key, std::string* dataOut);

class FurnitureConfigReader {
public:
	std::string ConfigPath;

	FurnitureConfigReader(std::string path) { this->ConfigPath = path; }
	const std::vector<FurnitureKeywordOffset>& ReadConfig();

private:
	std::string line;
	UInt32 lineIndex;

	std::string getNextData(char delimeter);
	char getNextChar();
};