#pragma once

namespace Utility {
	typedef void (*_ModPos)(TESObjectREFR*, UInt8, float);
	extern RelocAddr <_ModPos> ModPos;

	float GetActualScale(TESObjectREFR* refr);
	void SetScale(TESObjectREFR* refr, float scale);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);
	void trim(std::string& s);

	void ShowMessagebox(std::string asText);

	void BlockPlayerControls(bool block);

	void EnableMenuControls(BSInputEventUser* exception, std::map<BSInputEventUser*, bool>& enables, bool enabled);
}