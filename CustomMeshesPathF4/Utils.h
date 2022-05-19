#pragma once

class ExtraLeveledCreature : public BSExtraData {
public:
	TESForm* baseForm;		// 18
};

typedef ExtraLeveledCreature* (*_GetExtraLeveledCreature)(ExtraDataList*, UInt8);

void trim(std::string& s);
bool IsFileExists(const std::string& path);
TESForm* GetActorBaseForm(Actor* actor);
TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId);