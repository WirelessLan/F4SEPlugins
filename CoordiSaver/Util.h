#pragma once

namespace Util {
	std::vector<TESForm*> GetCurrentEquippedItemList(Actor *);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);
	void trim(std::string& s);
}