#include "Global.h"

void trim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

bool iequals(const std::string& a, const std::string& b) {
	return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char a, char b) {
		return tolower(a) == tolower(b);
	});
}

std::string toLower(const std::string& str) {
	std::string retStr = str;
	std::transform(retStr.begin(), retStr.end(), retStr.begin(), [](unsigned char c) {
		return std::tolower(c);
	});
	return retStr;
}

std::string remove_prefix(const std::string& prefixStr, const std::string& str) {
	size_t prefixPos = str.find(prefixStr);
	if (prefixPos == 0)
		return str.substr(prefixStr.length());
	return str;
}

void Log(const char* fmt, ...) {
	if (bDebug) {
		va_list args;
		va_start(args, fmt);
		gLog.Log(IDebugLog::kLevel_Message, fmt, args);
		va_end(args);
	}
}

bool IsFileExists(const std::string& path) {
	if (path.empty())
		return false;
	DWORD dwAttrib = GetFileAttributes(path.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::string GetFileExt(const std::string& fname) {
	size_t idx = fname.rfind('.');
	if (idx == std::string::npos)
		return std::string();
	return fname.substr(idx + 1);
}

typedef ExtraLeveledCreature* (*_GetExtraLeveledCreature)(ExtraDataList*, UInt8);
RelocAddr <_GetExtraLeveledCreature> GetExtraLeveledCreature_Internal(0x000436A0);

TESForm* GetActorBaseForm(Actor* actor) {
	if (!actor || !actor->extraDataList)
		return nullptr;

	ExtraLeveledCreature* extraLeveledCreature = GetExtraLeveledCreature_Internal(actor->extraDataList, 0x2D);
	return extraLeveledCreature ? extraLeveledCreature->baseForm : nullptr;
}

TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr) {
	UInt32 formID = std::stoul(formIdStr, nullptr, 16) & 0xFFFFFF;
	return GetFormFromIdentifier(pluginName, formID);
}

TESForm* GetFormFromIdentifier(const std::string& pluginName, const UInt32 formId) {
	if (!*g_dataHandler)
		return nullptr;

	const ModInfo* mod = (*g_dataHandler)->LookupModByName(pluginName.c_str());
	if (!mod || mod->modIndex == -1)
		return nullptr;

	UInt32 actualFormId = formId;
	UInt32 pluginIndex = mod->GetPartialIndex();
	if (!mod->IsLight())
		actualFormId |= pluginIndex << 24;
	else
		actualFormId |= pluginIndex << 12;

	return LookupFormByID(actualFormId);
}