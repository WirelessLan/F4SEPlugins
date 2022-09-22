#pragma once

enum RuleType {
	kRuleType_Actor,
	kRuleType_Race
};

bool ShouldLoadRules();
void LoadRules();
bool CheckCACSRule(RuleType type, UInt32 formId);
const std::string GetCACSPath(RuleType type, UInt32 formId);