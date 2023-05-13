#pragma once

namespace CACS {
	enum RuleType {
		kRuleType_Actor,
		kRuleType_Race
	};

	bool ShouldLoadRules();
	void LoadRules();
	bool CheckCACSRule(RuleType a_type, std::uint32_t a_formId);
	const std::string GetCACSPath(RuleType a_type, std::uint32_t a_formId);
	bool SetCustomPaths(RuleType a_type, std::uint32_t a_formId, const std::string& a_meshPath, const char* a_prefixPath, const char* a_subPath, std::string& o_prefixPath, std::string& o_subPath, std::string& o_fullPath);
}