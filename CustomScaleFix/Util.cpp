#include "Global.h"

RelocAddr <_GetScale> GetScale(0x3F8540);
RelocAddr <_SetScale> SetScale(0x3F85B0);

float GetFurnitureScale(TESObjectREFR* refr) {
	if (!refr)
		return 0.0f;

	float actualScale = GetScale(refr);

	NiAVObject* skeletonNode = refr->GetActorRootNode(false);
	if (!skeletonNode)
		return actualScale;

	static BSFixedString comNodeStr("COM");
	NiAVObject* comNode = skeletonNode->GetObjectByName(&comNodeStr);
	if (!comNode)
		return actualScale;

	actualScale *= comNode->m_localTransform.scale;

	NiAVObject* nodePtr = comNode->m_parent;
	while (nodePtr && nodePtr != skeletonNode) {
		actualScale *= nodePtr->m_localTransform.scale;
		nodePtr = nodePtr->m_parent;
	}

	return actualScale;
}

void ModifyFirstPersonScale(Actor* actor, float scale) {
	NiNode* rootNode = actor->GetActorRootNode(true);
	if (!rootNode)
		return;

	rootNode->m_localTransform.scale = scale;
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