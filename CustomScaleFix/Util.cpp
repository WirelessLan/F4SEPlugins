#include "Global.h"

RelocAddr <_GetScale> GetScale(0x3F8540);
RelocAddr <_SetScale> SetScale(0x3F85B0);

float GetFurnitureScale(TESObjectREFR* refr) {
	if (!refr)
		return 0.0f;

	float actualScale = GetScale(refr);

	NiAVObject* skeleton = refr->GetActorRootNode(false);
	if (!skeleton)
		return actualScale;

	static BSFixedString rootSurgeonNodeStr("RootSurgeonInserted");
	NiAVObject* rootSurgeonNode = skeleton->GetObjectByName(&rootSurgeonNodeStr);
	if (rootSurgeonNode)
		actualScale *= rootSurgeonNode->m_localTransform.scale;

	static BSFixedString rootNodeStr("Root");
	NiAVObject* rootNode = skeleton->GetObjectByName(&rootNodeStr);
	if (!rootNode)
		return actualScale;

	static BSFixedString comOverrideSurgeonNodeStr("COM_OverrideSurgeonInserted");
	NiAVObject* comOverrideSurgeonNode = rootNode->GetObjectByName(&comOverrideSurgeonNodeStr);
	if (comOverrideSurgeonNode)
		actualScale *= comOverrideSurgeonNode->m_localTransform.scale;

	static BSFixedString comOverrideNodeStr("COM_Override");
	NiAVObject* comOverrideNode = rootNode->GetObjectByName(&comOverrideNodeStr);
	if (!comOverrideNode)
		return actualScale;

	actualScale *= comOverrideNode->m_localTransform.scale;

	return actualScale;
}

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

bool HasKeyword(TESForm* form, BGSKeyword* keyword) {
	BGSKeywordForm* pKeywords = DYNAMIC_CAST(form, TESForm, BGSKeywordForm);
	if (!pKeywords)
		return false;

	for (UInt32 ii = 0; ii < pKeywords->numKeywords; ii++) {
		if (pKeywords->keywords[ii] == keyword)
			return true;
	}

	return false;
}

void trim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}