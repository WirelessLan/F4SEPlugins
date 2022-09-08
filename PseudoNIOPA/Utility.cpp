#include "Global.h"

namespace Utility {
	bool IsActorFrozen(Actor* actor) {
		return !(actor->uiFlags & (1 << 8));
	}

	void ToggleFreeze(Actor* actor) {
		if (!actor)
			return;

		if (!IsActorFrozen(actor))
			FreezeActor(actor);
		else
			UnfreezeActor(actor);
	}

	void FreezeActor(Actor* actor) {
		if (!actor)
			return;

		actor->uiFlags &= ~(1 << 8);

		if (actor == *g_player)
			InputEnableManager::SetInputEnableLayer();
	}

	void UnfreezeActor(Actor* actor) {
		if (!actor)
			return;

		actor->uiFlags |= 1 << 8;

		if (actor == *g_player)
			InputEnableManager::ResetInputEnableLayer();
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

	void Trim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
			}));
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}
};