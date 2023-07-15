#pragma once

namespace Utility {
	bool IsActorFrozen(Actor* a_actor);
	void ToggleFreeze(Actor* a_actor);
	void FreezeActor(Actor* a_actor);
	void UnfreezeActor(Actor* a_actor);
	Actor* GetCurrentConsoleActor();
	std::string Trim(std::string a_str);
};