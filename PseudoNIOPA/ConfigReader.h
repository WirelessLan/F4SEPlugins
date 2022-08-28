#pragma once
namespace ConfigReader {
	bool GetConfigValue(const char* section, const char* key, std::string* dataOut);
	std::vector<std::string> GetNodeListFromFile(std::string& groupName);
	bool SaveNodeData(const std::string& saveName, Actor* actor, const std::unordered_map<std::string, Nodes::NodeData>& nodeDataMap);
	std::unordered_map<std::string, Nodes::NodeData> LoadNodeData(const std::string& saveName);
	bool DeleteNodeData(const std::string& saveName);
};