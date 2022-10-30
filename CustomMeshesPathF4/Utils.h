#pragma once

class ExtraLeveledCreature : public BSExtraData {
public:
	TESForm* baseForm;		// 18
};

void trim(std::string& s);
bool iequals(const std::string& a, const std::string& b);
std::string toLower(const std::string& str);
std::string remove_prefix(const std::string& prefixStr, const std::string& str);
void Log(const char* fmt, ...);
bool IsFileExists(const std::string& path);
std::string GetFileExt(const std::string& fname);
TESForm* GetActorBaseForm(Actor* actor);
TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);