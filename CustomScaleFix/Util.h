#pragma once

typedef float (*_GetScale)(Actor*);
extern RelocAddr <_GetScale> GetScale;

TESForm* GetFormFromIdentifier(const std::string& identifier);
bool IsActorFemale(Actor* actor);
void SetRefScale(Actor* actor, float scale, bool isFemale);