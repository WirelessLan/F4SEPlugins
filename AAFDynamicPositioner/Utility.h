#pragma once

namespace Utility {
	typedef float (*_GetScale)(TESObjectREFR*);
	extern RelocAddr <_GetScale> GetScale;

	typedef float (*_SetScale)(TESObjectREFR*, float);
	extern RelocAddr <_SetScale> SetScale;

	typedef void (*_ModPos)(TESObjectREFR*, UInt8, float);
	extern RelocAddr <_ModPos> ModPos;

	float GetActualScale(TESObjectREFR* refr);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);
	void trim(std::string& s);

	void ShowMessagebox(std::string asText);

	void BlockPlayerControls(bool block);

	void EnableMenuControls(BSInputEventUser* exception, std::map<BSInputEventUser*, bool>& enables, bool enabled);
}