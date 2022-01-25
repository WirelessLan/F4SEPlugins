#include "Global.h"

inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}

inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

bool IsFileExists(const std::string& path) {
	if (path == "")
		return false;

	std::string fullPath = "Data\\" + path;

	DWORD dwAttrib = GetFileAttributes(fullPath.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

template <typename T>
T GetOffset(const void* baseObject, int offset) {
	return *reinterpret_cast<T*>((uintptr_t)baseObject + offset);
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
	UInt32 flags = GetOffset<UInt32>(mod, 0x334);
	if (flags & (1 << 9)) {
		actualFormId &= 0xFFF;
		actualFormId |= 0xFE << 24;
		actualFormId |= GetOffset<UInt16>(mod, 0x372) << 12;
	}
	else {
		actualFormId |= (mod->modIndex) << 24;
	}
	return LookupFormByID(actualFormId);
}