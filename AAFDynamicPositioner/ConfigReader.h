#pragma once

namespace ConfigReader {
	template<typename T>
	bool GetConfigValue(const char* section, const char* key, T* dataOut);
	template<>
	bool GetConfigValue(const char* section, const char* key, bool* dataOut);
	template<>
	bool GetConfigValue(const char* section, const char* key, SInt32* dataOut);
	template<>
	bool GetConfigValue(const char* section, const char* key, float* dataOut);
	template<>
	bool GetConfigValue(const char* section, const char* key, std::string* dataOut);
}