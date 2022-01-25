#pragma once

void trim(std::string& s);
bool IsFileExists(const std::string& path);
TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);