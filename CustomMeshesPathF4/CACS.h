#pragma once

enum RuleType {
	kRuleType_Actor,
	kRuleType_Race
};

bool ShouldLoadRules();
void LoadRules();
bool CheckCACSRule(UInt32 raceId, UInt32 actorId);
const std::string GetCACSPath(RuleType type, UInt32 formId);