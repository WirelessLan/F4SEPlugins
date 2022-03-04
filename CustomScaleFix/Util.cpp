#include "Global.h"

RelocAddr <_GetScale> GetScale(0x3F8540);
RelocAddr <_SetScale> SetScale(0x3F85B0);

bool IsActorFemale(Actor* actor) {
	TESNPC* actorBase = DYNAMIC_CAST(actor->baseForm, TESForm, TESNPC);
	if (!actorBase)
		return false;

	return CALL_MEMBER_FN(actorBase, GetSex)() == 1 ? true : false;
}

void SetRefScale(Actor* actor, float scale, bool isFemale) {
	if (!actor || !actor->race)
		return;

	float baseScale[2];
	memcpy(baseScale, actor->race->unk170, sizeof(float) * 2);

	float modifiedRefScale = std::round(scale * (100.0f / baseScale[isFemale]));
	std::uint16_t refScale = static_cast<std::uint16_t>(modifiedRefScale);
	memcpy(&actor->unk104, &refScale, sizeof std::uint16_t);
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

template <typename T>
T GetVirtualFunction(void* baseObject, int vtblIndex) {
	uintptr_t* vtbl = reinterpret_cast<uintptr_t**>(baseObject)[0];
	return reinterpret_cast<T>(vtbl[vtblIndex]);
}

bool HasKeyword(TESForm* form, BGSKeyword* keyword) {
	IKeywordFormBase* keywordFormBase = DYNAMIC_CAST(form, TESForm, IKeywordFormBase);
	if (keywordFormBase) {
		auto HasKeyword_Internal = GetVirtualFunction<_IKeywordFormBase_HasKeyword>(keywordFormBase, 1);
		if (HasKeyword_Internal(keywordFormBase, keyword, 0)) {
			return true;
		}
	}
	return false;
}

inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}