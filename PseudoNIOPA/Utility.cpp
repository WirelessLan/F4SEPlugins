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
	}

	void UnfreezeActor(Actor* actor) {
		if (!actor)
			return;

		actor->uiFlags |= 1 << 8;
	}
};