#include "Global.h"

namespace Utility {
	typedef float (*_GetScale)(TESObjectREFR*);
	extern RelocAddr <_GetScale> GetScale_Internal;
	RelocAddr <_GetScale> GetScale_Internal(0x3F8540);

	typedef float (*_SetScale)(TESObjectREFR*, float);
	extern RelocAddr <_SetScale> SetScale_Internal;
	RelocAddr <_SetScale> SetScale_Internal(0x3F85B0);

	RelocAddr <_ModPos> ModPos(0x4F3C30);

	float GetActualScale(TESObjectREFR* refr) {
		if (!refr)
			return 0.0f;

		float actualScale = GetScale_Internal(refr);

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

	void SetScale(TESObjectREFR* refr, float scale) {
		float currActualScale = GetActualScale(refr);
		if (currActualScale == scale)
			return;

		UInt16 refScale = reinterpret_cast<UInt16&>(refr->unk104);
		float baseScale = currActualScale * 100 / refScale;

		float modifiedRefScale = std::round(scale / baseScale * 100) / 100;
		SetScale_Internal(refr, modifiedRefScale);
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

	void trim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
			}));

		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	void ShowMessagebox(std::string asText) {
		CallGlobalFunctionNoWait1<BSFixedString>("Debug", "Messagebox", BSFixedString(asText.c_str()));
	}

	void BlockPlayerControls(bool block) {
		UInt8* g_pc = (UInt8 *)*g_playerControls;
		if (!g_pc)
			return;

		*(g_pc + 0x251) = block;
	}

	void EnableMenuControls(BSInputEventUser* exception, std::map<BSInputEventUser*, bool>& enableVec,  bool enabled) {
		if (!*g_menuControls)
			return;

		if (!enabled)
			enableVec.clear();

		// 0 ~ 7 is reserved
		for (UInt32 ii = 8; ii < (*g_menuControls)->inputEvents.count; ii++) {
			if ((*g_menuControls)->inputEvents.entries[ii] != exception) {
				if (!enabled) {
					enableVec.insert(std::make_pair((*g_menuControls)->inputEvents.entries[ii], (*g_menuControls)->inputEvents.entries[ii]->enabled));
					(*g_menuControls)->inputEvents.entries[ii]->enabled = enabled;
				}
				else {
					auto it = enableVec.find((*g_menuControls)->inputEvents.entries[ii]);
					if (it != enableVec.end())
						(*g_menuControls)->inputEvents.entries[ii]->enabled = it->second;
				}
			}
		}
	}
}