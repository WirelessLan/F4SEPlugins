#pragma once

template<typename T>
bool GetConfigValue(const char* section, const char* key, T* dataOut);
template<>
bool GetConfigValue(const char* section, const char* key, bool* dataOut);
template<>
bool GetConfigValue(const char* section, const char* key, UInt32* dataOut);
template<>
bool GetConfigValue(const char* section, const char* key, UInt16* dataOut);
template<>
bool GetConfigValue(const char* section, const char* key, std::string* dataOut);