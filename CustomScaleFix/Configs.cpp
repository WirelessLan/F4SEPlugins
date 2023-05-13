#include "Configs.h"

#include <fstream>

#include "Global.h"
#include "Utils.h"

namespace Configs {
	const std::string configPath("Data\\F4SE\\Plugins\\" PLUGIN_NAME ".ini");

	std::string GetConfigOption(const char* section, const char* key) {	
		char resultBuf[256] = { 0 };
		GetPrivateProfileString(section, key, NULL, resultBuf, sizeof(resultBuf), configPath.c_str());
		return resultBuf;
	}

	const char* GetTypeFormatting(bool dataOut) { return "%d"; }
	const char* GetTypeFormatting(std::uint32_t dataOut) { return "%u"; }

	bool GetConfigValue(const char* section, const char* key, bool* dataOut) {
		std::string	data = GetConfigOption(section, key);
		if (data.empty())
			return false;

		std::uint32_t tmp = 0;
		bool res = (sscanf_s(data.c_str(), GetTypeFormatting(tmp), &tmp) == 1);
		if (res)
			*dataOut = (tmp > 0);
		return res;
	}

	bool GetConfigValue(const char* section, const char* key, std::uint32_t* dataOut) {
		std::string	data = GetConfigOption(section, key);
		if (data.empty())
			return false;

		std::uint32_t tmp = 0;
		bool res = (sscanf_s(data.c_str(), GetTypeFormatting(tmp), &tmp) == 1);
		if (res)
			*dataOut = tmp;
		return res;
	}

	bool ConfigReader::getLine(std::fstream& a_file) {
		if (std::getline(a_file, _line)) {
			_lineIndex = 0;
			Utils::Trim(_line);

			return true;
		}

		return false;
	}

	bool ConfigReader::emptyOrComment() {
		if (_line.empty() || _line[0] == '#')
			return true;
		return false;
	}

	std::string ConfigReader::getNextData(char a_delimeter) {
		std::uint8_t ch;
		std::string retVal;

		while ((ch = getNextChar()) != 0xFF) {
			if (ch == '#') {
				undoGetNextChar();
				break;
			}

			if (a_delimeter != 0 && ch == a_delimeter)
				break;

			retVal += ch;
		}

		Utils::Trim(retVal);
		return retVal;
	}

	std::uint8_t ConfigReader::getNextChar() {
		if (_lineIndex < _line.length())
			return _line[_lineIndex++];

		return 0xFF;
	}

	void ConfigReader::undoGetNextChar() {
		if (_lineIndex > 0)
			_lineIndex--;
	}
}