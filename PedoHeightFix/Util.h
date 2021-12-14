#pragma once

float GetPlayerScale();
TESForm* GetFormFromIdentifier(const std::string& identifier);
bool isActorFemale(Actor* actor);
void SetActorRefScale(Actor* actor, float scale, bool isFemale);