#pragma once

void Trim(std::string& s);
std::string ToLower(const std::string& str);
std::string RemovePrefix(const std::string& prefixStr, const std::string& str);
void Log(const char* fmt, ...);
bool IsFileExists(const std::string& path);
std::string GetFileExt(const std::string& fname);
TESForm* GetActorBaseForm(Actor* actor);
TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::uint32_t formId);