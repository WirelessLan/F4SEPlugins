#pragma once

#include <string>

class TESObjectREFR;
class Actor;
class TESForm;
class BGSKeyword;

namespace Utils {
	float GetScale(TESObjectREFR* a_ref);
	float GetActualScale(TESObjectREFR* a_ref);
	void ModifyFirstPersonScale(Actor* a_actor, float a_scale);
	TESForm* GetFormFromIdentifier(const std::string& a_pluginName, const std::string& a_formId);
	TESForm* GetFormFromIdentifier(const std::string& a_pluginName, const std::uint32_t a_formId);
	bool HasKeyword(TESForm* form, BGSKeyword* keyword);
	void Trim(std::string& s);
}