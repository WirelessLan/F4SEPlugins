#pragma once

namespace Configs {
	bool GetConfigValue(const char* section, const char* key, bool* dataOut);
	bool GetConfigValue(const char* section, const char* key, std::uint32_t* dataOut);

	class ConfigReader {
	public:
		ConfigReader(std::string path) { _configPath = path; }

	protected:
		std::string _configPath;
		std::string _line;
		std::uint32_t _lineIndex;

		bool getLine(std::fstream& a_file);
		bool emptyOrComment();
		std::string getNextData(char delimeter);
		std::uint8_t getNextChar();
		void undoGetNextChar();
	};
}