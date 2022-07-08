#include "Global.h"

std::string GetConfigOption(const char* section, const char* key) {
	char	result[256] = { 0 };
	std::string configPath{ "Data\\F4SE\\Plugins\\" PLUGIN_NAME ".ini" };

	GetPrivateProfileString(section, key, NULL, result, sizeof(result), configPath.c_str());
	return result;
}

bool GetConfigValue(const char* section, const char* key, std::string* dataOut) {
	std::string	data = GetConfigOption(section, key);
	if (data.empty())
		return false;

	*dataOut = data;
	return true;
}