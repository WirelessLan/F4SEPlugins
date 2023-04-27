#pragma once

namespace CACS {
	enum RuleType {
		kRuleType_Actor,
		kRuleType_Race
	};

	bool ShouldLoadRules();
	void LoadRules();
	bool CheckCACSRule(RuleType type, std::uint32_t formId);
	const std::string GetCACSPath(RuleType type, std::uint32_t formId);
	bool SetCustomPaths(RuleType type, std::uint32_t formId, const std::string& meshPath, const char* prefixPath, const char* subPath, std::string& o_prefixPath, std::string& o_subPath, std::string& o_fullPath);
}