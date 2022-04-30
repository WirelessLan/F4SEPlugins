#pragma once

namespace Utility {
	float GetActualScale(TESObjectREFR* refr);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);
	bool IsAAFDoppelganger(Actor* actor);
	Actor* GetAAFDoppelganer();
	void trim(std::string& s);
}