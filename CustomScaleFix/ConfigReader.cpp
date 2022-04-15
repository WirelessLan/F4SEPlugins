#include "Global.h"

const std::string& GetConfigPath(void) {
	static std::string s_configPath;

	if (s_configPath.empty()) {
		std::string subPath{ "Data\\F4SE\\Plugins\\" PLUGIN_NAME ".ini" };
		s_configPath = subPath;
	}

	return s_configPath;
}

std::string GetConfigOption(const char* section, const char* key) {
	std::string	result;

	const std::string& configPath = GetConfigPath();
	if (!configPath.empty()) {
		char	resultBuf[256] = { 0 };
		UInt32	resultLen = GetPrivateProfileString(section, key, NULL, resultBuf, sizeof(resultBuf), configPath.c_str());
		result = resultBuf;
	}

	return result;
}

template<typename T>
const char* GetTypeFormatting(T* dataOut) {
	return false;
}

template<> const char* GetTypeFormatting(double* dataOut) { return "%lf"; }
template<> const char* GetTypeFormatting(float* dataOut) { return "%f"; }
template<> const char* GetTypeFormatting(bool* dataOut) { return "%d"; }
template<> const char* GetTypeFormatting(SInt16* dataOut) { return "%hd"; }
template<> const char* GetTypeFormatting(UInt16* dataOut) { return "%hu"; }
template<> const char* GetTypeFormatting(SInt32* dataOut) { return "%d"; }
template<> const char* GetTypeFormatting(UInt32* dataOut) { return "%u"; }
template<> const char* GetTypeFormatting(UInt64* dataOut) { return "%I64u"; }

template<typename T>
bool GetConfigValue(const char* section, const char* key, T* dataOut) {
	std::string	data = GetConfigOption(section, key);
	if (data.empty())
		return false;

	T tmp;
	bool res = (sscanf_s(data.c_str(), GetTypeFormatting(dataOut), &tmp) == 1);
	if (res)
		*dataOut = tmp;
	return res;
}

template<>
bool GetConfigValue(const char* section, const char* key, bool* dataOut) {
	std::string	data = GetConfigOption(section, key);
	if (data.empty())
		return false;

	UInt32 tmp;
	bool res = (sscanf_s(data.c_str(), GetTypeFormatting(&tmp), &tmp) == 1);
	if (res)
		*dataOut = (tmp > 0);
	return res;
}

template<>
bool GetConfigValue(const char* section, const char* key, UInt32* dataOut) {
	std::string	data = GetConfigOption(section, key);
	if (data.empty())
		return false;

	UInt32 tmp;
	bool res = (sscanf_s(data.c_str(), GetTypeFormatting(&tmp), &tmp) == 1);
	if (res)
		*dataOut = tmp;
	return res;
}

template<>
bool GetConfigValue(const char* section, const char* key, std::string* dataOut) {
	std::string	data = GetConfigOption(section, key);
	if (data.empty())
		return false;

	*dataOut = data;
	return true;
}

const std::vector<FurnitureKeywordOffset>& FurnitureConfigReader::ReadConfig() {
	static std::vector<FurnitureKeywordOffset> configs;
	
	if (configs.size())
		configs.clear();

	std::fstream configFile(ConfigPath);
	if (!configFile.is_open()) {
		_MESSAGE("Can't read furniture config file! [%s]", ConfigPath.c_str());
		return configs;
	}

	std::string pluginName, formId, horOffset, verOffset;
	while (std::getline(configFile, line)) {
		lineIndex = 0;

		trim(line);
		if (line.empty() || line[0] == '#')
			continue;

		pluginName = getNextData('|');
		if (pluginName.empty()) {
			_MESSAGE("Can't read Plugin Name[%s]", line.c_str());
			continue;
		}

		formId = getNextData('|');
		if (formId.empty()) {
			_MESSAGE("Can't read Form ID[%s]", line.c_str());
			continue;
		}

		horOffset = getNextData(',');
		if (horOffset.empty()) {
			_MESSAGE("Can't read horOffset[%s]", line.c_str());
			continue;
		}

		verOffset = getNextData(0);
		if (verOffset.empty()) {
			_MESSAGE("Can't read verOffset[%s]", line.c_str());
			continue;
		}

		TESForm* keyword = GetFormFromIdentifier(pluginName, formId);
		if (!keyword) {
			_MESSAGE("Can't find Keyword[%s]", line.c_str());
			continue;
		}

		configs.push_back({ (BGSKeyword*)keyword, stof(horOffset), stof(verOffset)});
	}
	configFile.close();

	return configs;
}

std::string FurnitureConfigReader::getNextData(char delimeter) {
	char ch;
	std::string retVal = "";

	while ((ch = getNextChar()) > 0) {
		if (ch == '#') {
			undoGetNextChar();
			break;
		}

		if (delimeter != 0 && ch == delimeter)
			break;

		retVal += ch;
	}

	trim(retVal);
	return retVal;
}

char FurnitureConfigReader::getNextChar() {
	if (lineIndex < line.length())
		return line[lineIndex++];

	return -1;
}

void FurnitureConfigReader::undoGetNextChar() {
	if (lineIndex > 0)
		lineIndex--;
}