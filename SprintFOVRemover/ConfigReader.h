#pragma once

template<typename T>
bool GetConfigValue(const char* section, const char* key, T* dataOut);
template<>
bool GetConfigValue(const char* section, const char* key, bool* dataOut);
bool GetConfigValue(const char* section, const char* key, UInt32* dataOut);