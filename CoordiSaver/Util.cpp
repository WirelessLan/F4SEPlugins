#include "Global.h"

namespace Util {
	bool HasItem(Actor* actor, TESForm* item) {
		if (!actor || !item)
			return 0;

		BGSInventoryList* inventory = actor->inventoryList;
		if (!inventory)
			return 0;

		UInt32 totalItemCount = 0;
		inventory->inventoryLock.LockForRead();
		for (UInt32 ii = 0; ii < inventory->items.count; ii++) {
			if (inventory->items[ii].form == item) {
				BGSInventoryItem::Stack* sp = inventory->items[ii].stack;
				while (sp) {
					totalItemCount += sp->count;
					sp = sp->next;
				}
				break;
			}
		}
		inventory->inventoryLock.UnlockRead();

		return totalItemCount > 0;
	}

	std::vector<TESForm*> GetCurrentEquippedItemList(Actor* actor) {
		std::vector<TESForm*> result;

		if (!actor || !actor->equipData)
			return result;

		// 30 (0) ~ 58 (28) Slots
		for (UInt32 ii = 0; ii <= 28; ii++) {
			if (actor->equipData->slots[ii].item && HasItem(actor, actor->equipData->slots[ii].item))
				result.push_back(actor->equipData->slots[ii].item);
		}

		return result;
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

	Actor* GetCurrentConsoleActor() {
		UInt32 handle = (*g_consoleHandle);
		NiPointer<TESObjectREFR> refr;
		if (handle != 0 && handle != (*g_invalidRefHandle)) {
			LookupREFRByHandle(handle, refr);
			return DYNAMIC_CAST(refr, TESObjectREFR, Actor);
		}

		return nullptr;
	}

	void LoadSlot(std::string& slotName) {
		CallGlobalFunctionNoWait1<BSFixedString>("CoordiSaver", "LoadSlot", BSFixedString(slotName.c_str()));
	}
}