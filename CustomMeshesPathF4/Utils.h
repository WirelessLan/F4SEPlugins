#pragma once

class TESForm;
class Actor;

namespace Utils {
	void Trim(std::string& s);
	std::string ToLower(const std::string& str);
	std::string ReplaceSlash(const std::string& input);
	bool StartsWith(const std::string& input, const std::string& prefix);
	std::string RemovePrefix(const std::string& input, const std::string& prefix);
	void Log(const char* fmt, ...);
	bool IsFileExists(const std::string& path);
	std::string GetFileExt(const std::string& fname);
	std::uint32_t GetBaseFormID(Actor* actor);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
	TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::uint32_t formId);
}