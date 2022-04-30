#include "Global.h"

namespace Utility {
	RelocAddr <_GetScale> GetScale(0x3F8540);
	RelocAddr <_SetScale> SetScale(0x3F85B0);

	float GetActualScale(TESObjectREFR* refr) {
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

	bool IsPrewarNora(UInt32 formId, UInt32 baseFormId) {
		// BaseForm의 ID가 일치하는 레퍼런스 NPC가 둘 이상 존재할 수 있으므로
		// 바닐라 레퍼런스 NPC인지 확인하여 무시
		return formId != 0xA7D38 && baseFormId == 0xA7D35;
	}

	bool IsAAFDoppelganger(UInt32 formId) {
		static TESForm* aaf_doppForm = Utility::GetFormFromIdentifier("AAF.esm", 0x72E2);
		if (!aaf_doppForm)
			return false;

		return formId == aaf_doppForm->formID;
	}

	bool IsAAFDoppelganger(Actor* actor) {
		if (!actor || !actor->baseForm)
			return false;

		bool isFemale = false;
		TESNPC* actorBase = DYNAMIC_CAST(actor->baseForm, TESForm, TESNPC);
		if (actorBase)
			isFemale = CALL_MEMBER_FN(actorBase, GetSex)() == 1 ? true : false;

		if (isFemale) {
			if (IsPrewarNora(actor->formID, actor->baseForm->formID) || IsAAFDoppelganger(actor->baseForm->formID))
				return true;
		}
		else if (IsAAFDoppelganger(actor->baseForm->formID)) {
			return true;
		}

		return false;
	}

	Actor* GetAAFDoppelganer() {
		if (*g_player && (*g_player)->parentCell) {
			for (UInt32 ii = 0; ii < (*g_player)->parentCell->objectList.count; ii++) {
				TESObjectREFR* ref = (*g_player)->parentCell->objectList.entries[ii];
				Actor* actorRef = DYNAMIC_CAST(ref, TESObjectREFR, Actor);
				if (actorRef && IsAAFDoppelganger(actorRef))
					return actorRef;
			}
		}

		return nullptr;
	}

	void trim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
			}));

		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}
}