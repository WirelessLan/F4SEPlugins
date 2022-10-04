#pragma once

namespace Utility {
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);
	void Trim(std::string& s);
}