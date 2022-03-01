#pragma once

typedef float (*_GetScale)(TESObjectREFR*);
extern RelocAddr <_GetScale> GetScale;

typedef float (*_SetScale)(TESObjectREFR*, float);
extern RelocAddr <_SetScale> SetScale;

bool IsActorFemale(Actor* actor);
void SetRefScale(Actor* actor, float scale, bool isFemale);

TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);