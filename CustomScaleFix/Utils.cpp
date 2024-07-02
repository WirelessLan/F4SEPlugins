#include "Utils.h"

#include <f4se/GameRTTI.h>
#include <f4se/GameData.h>
#include <f4se/GameReferences.h>
#include <f4se/NiNodes.h>

namespace Utils {
	float GetScale(TESObjectREFR* a_ref) {
		using func_t = float(*)(TESObjectREFR*);
		func_t func = RelocAddr<func_t>(0x4AB5F0);
		return func(a_ref);
	}

	void SetScale(TESObjectREFR* a_ref, float a_scale) {
		using func_t = float(*)(TESObjectREFR*, float);
		func_t func = RelocAddr<func_t>(0x50FAC0);
		func(a_ref, a_scale);
	}

	float GetActualScale(TESObjectREFR* a_ref) {
		if (!a_ref)
			return 0.0f;

		float retScale = GetScale(a_ref);

		NiAVObject* skeletonNode = a_ref->GetActorRootNode(false);
		if (!skeletonNode)
			return retScale;

		static const BSFixedString comNodeStr("COM");
		NiAVObject* comNode = skeletonNode->GetObjectByName(&comNodeStr);
		if (!comNode)
			return retScale;

		retScale *= comNode->m_localTransform.scale;

		NiAVObject* nodePtr = comNode->m_parent;
		while (nodePtr && nodePtr != skeletonNode) {
			retScale *= nodePtr->m_localTransform.scale;
			nodePtr = nodePtr->m_parent;
		}

		return retScale;
	}

	void ModifyFirstPersonScale(Actor* a_actor, float a_scale) {
		NiNode* rootNode = a_actor->GetActorRootNode(true);
		if (!rootNode)
			return;

		rootNode->m_localTransform.scale = a_scale;
	}

	TESForm* GetFormFromIdentifier(const std::string& a_pluginName, const std::string& a_formId) {
		std::uint32_t formID = std::stoul(a_formId, nullptr, 16) & 0xFFFFFF;
		return GetFormFromIdentifier(a_pluginName, formID);
	}

	TESForm* GetFormFromIdentifier(const std::string& a_pluginName, const std::uint32_t a_formId) {
		if (!*g_dataHandler)
			return nullptr;

		const ModInfo* mod = (*g_dataHandler)->LookupModByName(a_pluginName.c_str());
		if (!mod || mod->modIndex == -1)
			return nullptr;

		std::uint32_t actualFormId = a_formId;
		std::uint32_t pluginIndex = mod->GetPartialIndex();
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

		for (std::uint32_t ii = 0; ii < pKeywords->numKeywords; ii++) {
			if (pKeywords->keywords[ii] == keyword)
				return true;
		}

		return false;
	}

	void Trim(std::string& a_str) {
		a_str.erase(a_str.begin(), std::find_if(a_str.begin(), a_str.end(), [](unsigned char ch) {
			return !std::isspace(ch);
		}));
		a_str.erase(std::find_if(a_str.rbegin(), a_str.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
		}).base(), a_str.end());
	}
}