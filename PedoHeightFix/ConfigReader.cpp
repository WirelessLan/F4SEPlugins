#include "Global.h"

const std::string& GetConfigPath(void) {
	static std::string s_configPath;

	if (s_configPath.empty()) {
		std::string subPath{ "Data\\F4SE\\Plugins\\" PLUGIN_NAME ".ini" };
		s_configPath = subPath;
		_MESSAGE("config path = %s", s_configPath.c_str());
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
template<> const char* GetTypeFormatting(bool* dataOut) { return "%c"; }
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
bool GetConfigValue(const char* section, const char* key, std::string* dataOut) {
	std::string	data = GetConfigOption(section, key);
	if (data.empty())
		return false;

	*dataOut = data;
	return true;
}