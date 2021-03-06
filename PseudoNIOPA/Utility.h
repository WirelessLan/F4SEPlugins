#pragma once

namespace Utility {
	bool IsActorFrozen(Actor* actor);
	void ToggleFreeze(Actor* actor);
	void FreezeActor(Actor* actor);
	void UnfreezeActor(Actor* actor);
	void Trim(std::string& s);
};