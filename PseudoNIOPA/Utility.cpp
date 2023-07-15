#include "Global.h"

namespace Utility {
	bool IsActorFrozen(Actor* a_actor) {
		return !(a_actor->uiFlags & (1 << 8));
	}

	void ToggleFreeze(Actor* a_actor) {
		if (!a_actor)
			return;

		if (!IsActorFrozen(a_actor))
			FreezeActor(a_actor);
		else
			UnfreezeActor(a_actor);
	}

	void FreezeActor(Actor* a_actor) {
		if (!a_actor)
			return;

		a_actor->uiFlags &= ~(1 << 8);

		if (a_actor == *g_player)
			InputEnableManager::SetInputEnableLayer();
	}

	void UnfreezeActor(Actor* a_actor) {
		if (!a_actor)
			return;

		a_actor->uiFlags |= 1 << 8;

		if (a_actor == *g_player)
			InputEnableManager::ResetInputEnableLayer();
	}

	Actor* GetCurrentConsoleActor() {
		std::uint32_t handle = (*g_consoleHandle);
		if (handle == 0 || handle == *g_invalidRefHandle)
			return nullptr;

		NiPointer<TESObjectREFR> refr;
		LookupREFRByHandle(handle, refr);
		return DYNAMIC_CAST(refr, TESObjectREFR, Actor);
	}

	std::string Trim(std::string a_str) {
		a_str.erase(a_str.begin(), std::find_if(a_str.begin(), a_str.end(), [](int ch) {
			return !std::isspace(ch);
			}));
		a_str.erase(std::find_if(a_str.rbegin(), a_str.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), a_str.end());
		return a_str;
	}
};