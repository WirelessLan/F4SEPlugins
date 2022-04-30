#pragma once

typedef float (*_GetScale)(TESObjectREFR*);
extern RelocAddr <_GetScale> GetScale;

typedef float (*_SetScale)(TESObjectREFR*, float);
extern RelocAddr <_SetScale> SetScale;

float GetFurnitureScale(TESObjectREFR* refr);

void ModifyFirstPersonScale(Actor* actor, float scale);

TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);

TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);

bool HasKeyword(TESForm* form, BGSKeyword* keyword);

void trim(std::string& s);