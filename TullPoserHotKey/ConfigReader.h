#pragma once

namespace ConfigReader {
	std::vector<std::string> ReadConfig();
	std::vector<std::string> ReadPluginPoseConfig(const std::string& pluginName);
	bool ShouldReadConfig();
	bool ShouldReadPluginPoseConfig(const std::string& pluginName);
}