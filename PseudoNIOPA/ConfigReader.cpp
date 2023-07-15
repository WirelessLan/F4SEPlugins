#include "Global.h"

namespace ConfigReader {
	std::string GetConfigOption(const char* a_section, const char* a_key) {
		char	result[256] = { 0 };
		std::string configPath{ "Data\\F4SE\\Plugins\\" PLUGIN_NAME ".ini" };

		GetPrivateProfileString(a_section, a_key, NULL, result, sizeof(result), configPath.c_str());
		return result;
	}

	bool GetConfigValue(const char* a_section, const char* a_key, std::string* a_dataOut) {
		std::string	data = GetConfigOption(a_section, a_key);
		if (data.empty())
			return false;

		*a_dataOut = data;
		return true;
	}

	std::vector<std::string> GetNodeListFromFile(std::string& a_groupName) {
		std::vector<std::string> retVec;

		std::string configPath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Groups\\" + a_groupName + ".txt";
		FileReader reader(configPath);
		if (!reader.Open()) {
			_MESSAGE("Cannot open a config file: %s", configPath.c_str());
			return retVec;
		}

		std::string nodeName;
		while (reader.GetLine()) {
			if (reader.EmptyOrComment())
				continue;

			nodeName = reader.GetNextData(0);
			if (nodeName.empty()) {
				_MESSAGE("Cannot read a Node Name: %s", reader.GetCurrentLine().c_str());
				continue;
			}

			retVec.push_back(nodeName);
		}

		return retVec;
	}

	std::vector<std::string> GetNodeDataList() {
		std::vector<std::string> retVec;

		std::string directory = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Saves\\";
		const char* match = "*.pnop";

		for (IDirectoryIterator iter(directory.c_str(), match); !iter.Done(); iter.Next()) {
			WIN32_FIND_DATA* fileData = iter.Get();

			std::string s_fileName = fileData->cFileName;
			size_t lastDotIdx = s_fileName.find_last_of(".");
			std::string rawName = s_fileName.substr(0, lastDotIdx);

			retVec.push_back(rawName);
		}

		return retVec;
	}

	bool SaveNodeData(const std::string& a_saveName, Actor* a_actor, const std::unordered_map<std::string, Nodes::NodeData>& a_nodeDataMap) {
		std::string savePath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Saves\\" + a_saveName + ".pnop";
		std::ofstream saveFile(savePath);

		if (!saveFile.is_open())
			return false;

		for (const auto& nodeDataPair : a_nodeDataMap) {
			const std::string& nodeName = nodeDataPair.first;
			const Nodes::NodeData& nodeData = nodeDataPair.second;

			NiNode* node = Nodes::GetNode(a_actor, nodeName);
			if (!node)
				continue;



			if (nodeData.posModified) {
				saveFile << nodeName << "|" << "Position" << "|" << node->m_localTransform.pos.x << ","
						<< node->m_localTransform.pos.y << "," << node->m_localTransform.pos.z << std::endl;
			}

			if (nodeData.rotModified) {
				float y, p, r;
				node->m_localTransform.rot.GetEulerAngles(&y, &p, &r);
				saveFile << nodeName << "|" << "Rotation" << "|" << y << "," << p << "," << r << std::endl;
			}

			if (nodeData.scaleModified) {
				saveFile << nodeName << "|" << "Scale" << "|" << node->m_localTransform.scale << std::endl;
			}
		}

		return true;
	}

	std::unordered_map<std::string, Nodes::NodeData> LoadNodeData(const std::string& a_saveName) {
		std::unordered_map<std::string, Nodes::NodeData> retMap;

		std::string savePath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Saves\\" + a_saveName + ".pnop";
		FileReader reader(savePath);
		if (!reader.Open())
			return retMap;

		std::string nodeName, propertyName, xValue, yValue, zValue;
		while (reader.GetLine()) {
			if (reader.EmptyOrComment())
				continue;

			nodeName = reader.GetNextData('|');
			if (nodeName.empty()) {
				_MESSAGE("Cannot read a Node Name: %s", reader.GetCurrentLine().c_str());
				continue;
			}

			propertyName = reader.GetNextData('|');
			if (propertyName.empty()) {
				_MESSAGE("Cannot read a Property Name: %s", reader.GetCurrentLine().c_str());
				continue;
			}

			if (propertyName == "Position" || propertyName == "Rotation") {
				xValue = reader.GetNextData(',');
				if (xValue.empty()) {
					_MESSAGE("Cannot read a X(Y) Value: %s", reader.GetCurrentLine().c_str());
					continue;
				}

				yValue = reader.GetNextData(',');
				if (xValue.empty()) {
					_MESSAGE("Cannot read a Y(P) Value: %s", reader.GetCurrentLine().c_str());
					continue;
				}

				zValue = reader.GetNextData(0);
				if (xValue.empty()) {
					_MESSAGE("Cannot read a Z(R) Value: %s", reader.GetCurrentLine().c_str());
					continue;
				}
				
				auto& nodeData = retMap[nodeName];
				if (propertyName == "Position") {
					nodeData.posModified = true;
					nodeData.pos.x = stof(xValue);
					nodeData.pos.y = stof(yValue);
					nodeData.pos.z = stof(zValue);
				}
				else {
					nodeData.rotModified = true;
					nodeData.rot.y = stof(xValue);
					nodeData.rot.p = stof(yValue);
					nodeData.rot.r = stof(zValue);
				}
			}
			else if (propertyName == "Scale") {
				xValue = reader.GetNextData(0);
				if (xValue.empty()) {
					_MESSAGE("Cannot read a Scale Value: %s", reader.GetCurrentLine().c_str());
					continue;
				}

				auto& nodeData = retMap[nodeName];
				nodeData.scaleModified = true;
				nodeData.scale = stof(xValue);
			}
			else {
				_MESSAGE("Invalid Property Name: %s", reader.GetCurrentLine().c_str());
				continue;
			}
		}

		return retMap;
	}

	bool DeleteNodeData(const std::string& a_saveName) {
		std::string savePath = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Saves\\" + a_saveName + ".pnop";
		return remove(savePath.c_str()) == 0;
	}
}