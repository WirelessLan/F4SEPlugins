#pragma once

namespace Utility {
	typedef float (*_GetScale)(TESObjectREFR*);
	extern RelocAddr <_GetScale> GetScale;

	typedef float (*_SetScale)(TESObjectREFR*, float);
	extern RelocAddr <_SetScale> SetScale;

	float GetActualScale(TESObjectREFR* refr);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);
	void trim(std::string& s);
}