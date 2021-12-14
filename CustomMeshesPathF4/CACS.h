#pragma once

enum RuleType {
	ActorType,
	RaceType
};

void InitCACS();
bool CheckCACSRule(UInt32 raceId, UInt32 actorId);
const std::string GetCACSPath(RuleType type, UInt32 formId);