#include "Global.h"

namespace ConfigReader {
	std::string GetConfigOption(const char* section, const char* key) {
		char	result[256] = { 0 };
		std::string configPath{ "Data\\MCM\\Settings\\" PLUGIN_NAME ".ini" };

		GetPrivateProfileString(section, key, NULL, result, sizeof(result), configPath.c_str());
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

		bool tmp;
		bool res = (sscanf_s(data.c_str(), GetTypeFormatting(&tmp), &tmp) == 1);
		if (res)
			*dataOut = tmp;
		return res;
	}

	template<>
	bool GetConfigValue(const char* section, const char* key, SInt32* dataOut) {
		std::string	data = GetConfigOption(section, key);
		if (data.empty())
			return false;

		SInt32 tmp;
		bool res = (sscanf_s(data.c_str(), GetTypeFormatting(&tmp), &tmp) == 1);
		if (res)
			*dataOut = tmp;
		return res;
	}

	template<>
	bool GetConfigValue(const char* section, const char* key, float* dataOut) {
		std::string	data = GetConfigOption(section, key);
		if (data.empty())
			return false;

		float tmp;
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
}