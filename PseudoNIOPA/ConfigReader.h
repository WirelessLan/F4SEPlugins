#pragma once

namespace ConfigReader {
	class FileReader {
	public:
		FileReader(std::string path) { _filePath = path; }

		~FileReader() {
			_fileStream.close();
		}

		bool Open() {
			_fileStream = std::fstream(_filePath);
			if (!_fileStream.is_open())
				return false;
			return true;
		}

		bool GetLine() {
			if (std::getline(_fileStream, _line)) {
				_lineIndex = 0;
				_line = Utility::Trim(_line);
				return true;
			}
			return false;
		}

		bool EmptyOrComment() {
			if (_line.empty() || _line[0] == '#')
				return true;
			return false;
		}

		std::string GetNextData(char a_delimeter) {
			std::uint8_t ch;
			std::string retVal;

			while ((ch = getNextChar()) != 0xFF) {
				if (ch == '#') {
					if (_lineIndex > 0) _lineIndex--;
					break;
				}

				if (a_delimeter != 0 && ch == a_delimeter)
					break;

				retVal += ch;
			}

			return Utility::Trim(retVal);
		}

		std::string GetCurrentLine() {
			return _line;
		}

		std::uint32_t GetCurrentLineIndex() {
			return _lineIndex;
		}

	private:
		std::uint8_t getNextChar() {
			if (_lineIndex < _line.length())
				return _line[_lineIndex++];
			return 0xFF;
		}

		std::string _filePath;
		std::fstream _fileStream;
		std::string _line;
		std::uint32_t _lineIndex;
	};

	bool GetConfigValue(const char* a_section, const char* a_key, std::string* a_dataOut);
	std::vector<std::string> GetNodeListFromFile(std::string& a_groupName);
	std::vector<std::string> GetNodeDataList();
	bool SaveNodeData(const std::string& a_saveName, Actor* a_actor, const std::unordered_map<std::string, Nodes::NodeData>& a_nodeDataMap);
	std::unordered_map<std::string, Nodes::NodeData> LoadNodeData(const std::string& a_saveName);
	bool DeleteNodeData(const std::string& a_saveName);
};