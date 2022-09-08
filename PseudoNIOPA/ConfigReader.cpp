#include "Global.h"

namespace ConfigReader {
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

	char GetNextChar(const std::string& line, UInt32& index) {
		if (index < line.length())
			return line[index++];

		return -1;
	}

	std::string GetNextData(const std::string& line, UInt32& index, char delimeter) {
		char ch;
		std::string retVal = "";

		while ((ch = GetNextChar(line, index)) > 0) {
			if (ch == '#') {
				if (index > 0) index--;
				break;
			}

			if (delimeter != 0 && ch == delimeter)
				break;

			retVal += ch;
		}

		Utility::Trim(retVal);
		return retVal;
	}

	std::vector<std::string> GetNodeListFromFile(std::string& groupName) {
		std::string configPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Groups\\" + groupName + ".txt";
		std::ifstream configFile(configPath);
		std::vector<std::string> retVec;

		if (!configFile.is_open()) {
			_MESSAGE("Cannot open a config file! [%s]", configPath.c_str());
			return retVec;
		}

		std::string line;
		std::string nodeName;
		while (std::getline(configFile, line)) {
			Utility::Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			UInt32 index = 0;

			nodeName = GetNextData(line, index, 0);
			if (nodeName.empty()) {
				_MESSAGE("Cannot read a Node Name[%s]", line.c_str());
				continue;
			}

			retVec.push_back(nodeName);
		}

		return retVec;
	}

	std::vector<std::string> GetNodeDataList() {
		std::string directory = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Saves\\";
		const char* match = "*.pnop";
		std::vector<std::string> retVec;

		for (IDirectoryIterator iter(directory.c_str(), match); !iter.Done(); iter.Next()) {
			WIN32_FIND_DATA* fileData = iter.Get();

			std::string s_fileName = fileData->cFileName;
			size_t lastDotIdx = s_fileName.find_last_of(".");
			std::string rawName = s_fileName.substr(0, lastDotIdx);

			retVec.push_back(rawName);
		}

		return retVec;
	}

	bool SaveNodeData(const std::string& saveName, Actor* actor, const std::unordered_map<std::string, Nodes::NodeData>& nodeDataMap) {
		std::string savePath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Saves\\" + saveName + ".pnop";
		std::ofstream saveFile(savePath);

		if (!saveFile.is_open())
			return false;

		for (auto& iter : nodeDataMap) {
			NiNode* node = Nodes::GetNode(actor, iter.first);
			if (!node)
				continue;

			if (iter.second.posModified) {
				saveFile << iter.second.nodeName << "|" << "Position" << "|" << node->m_localTransform.pos.x << "," << node->m_localTransform.pos.y << "," << node->m_localTransform.pos.z << std::endl;
			}
			if (iter.second.rotModified) {
				float y, p, r;
				node->m_localTransform.rot.GetEulerAngles(&y, &p, &r);
				saveFile << iter.second.nodeName << "|" << "Rotation" << "|" << y << "," << p << "," << r << std::endl;
			}
			if (iter.second.scaleModified) {
				saveFile << iter.second.nodeName << "|" << "Scale" << "|" << node->m_localTransform.scale << std::endl;
			}
		}

		return true;
	}

	std::unordered_map<std::string, Nodes::NodeData> LoadNodeData(const std::string& saveName) {
		std::string savePath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Saves\\" + saveName + ".pnop";
		std::ifstream saveFile(savePath);
		std::unordered_map<std::string, Nodes::NodeData> retMap;

		if (!saveFile.is_open())
			return retMap;

		std::string line;
		std::string nodeName, propertyName, xValue, yValue, zValue;
		while (std::getline(saveFile, line)) {
			Utility::Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			UInt32 index = 0;

			nodeName = GetNextData(line, index, '|');
			if (nodeName.empty()) {
				_MESSAGE("Cannot read a Node Name: %s", line.c_str());
				continue;
			}

			propertyName = GetNextData(line, index, '|');
			if (propertyName.empty()) {
				_MESSAGE("Cannot read a Property Name: %s", line.c_str());
				continue;
			}

			if (propertyName == "Position" || propertyName == "Rotation") {
				xValue = GetNextData(line, index, ',');
				if (xValue.empty()) {
					_MESSAGE("Cannot read a X(Y) Value: %s", line.c_str());
					continue;
				}

				yValue = GetNextData(line, index, ',');
				if (xValue.empty()) {
					_MESSAGE("Cannot read a Y(P) Value: %s", line.c_str());
					continue;
				}

				zValue = GetNextData(line, index, 0);
				if (xValue.empty()) {
					_MESSAGE("Cannot read a Z(R) Value: %s", line.c_str());
					continue;
				}

				auto it = retMap.find(nodeName);
				if (it == retMap.end()) {
					Nodes::NodeData newNode = {};
					newNode.nodeName = nodeName;
					if (propertyName == "Position") {
						newNode.posModified = true;
						newNode.pos.x = stof(xValue);
						newNode.pos.y = stof(yValue);
						newNode.pos.z = stof(zValue);
					}
					else {
						newNode.rotModified = true;
						newNode.rot.y = stof(xValue);
						newNode.rot.p = stof(yValue);
						newNode.rot.r = stof(zValue);
					}
					retMap.insert(std::make_pair(nodeName, newNode));
				}
				else {
					if (propertyName == "Position") {
						it->second.posModified = true;
						it->second.pos.x = stof(xValue);
						it->second.pos.y = stof(yValue);
						it->second.pos.z = stof(zValue);
					}
					else {
						it->second.rotModified = true;
						it->second.rot.y = stof(xValue);
						it->second.rot.p = stof(yValue);
						it->second.rot.r = stof(zValue);
					}
				}
			}
			else if (propertyName == "Scale") {
				xValue = GetNextData(line, index, 0);
				if (xValue.empty()) {
					_MESSAGE("Cannot read a Scale Value: %s", line.c_str());
					continue;
				}

				auto it = retMap.find(nodeName);
				if (it == retMap.end()) {
					Nodes::NodeData newNode = {};
					newNode.nodeName = nodeName;
					newNode.scaleModified = true;
					newNode.scale = stof(xValue);
					retMap.insert(std::make_pair(nodeName, newNode));
				}
				else {
					it->second.scaleModified = true;
					it->second.scale = stof(xValue);
				}
			}
			else {
				_MESSAGE("Invalid Property Name: %s", line.c_str());
				continue;
			}
		}

		return retMap;
	}

	bool DeleteNodeData(const std::string& saveName) {
		std::string savePath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Saves\\" + saveName + ".pnop";
		return remove(savePath.c_str()) == 0;
	}
}