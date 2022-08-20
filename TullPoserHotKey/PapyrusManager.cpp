#include "Global.h"

namespace PapyrusManager {
	void ShowMenu(StaticFunctionTag*) {
		ScaleformManager::TullPoserHotKeyMenu::OpenMenu();
	}

	void StopIdle(StaticFunctionTag*) {
		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor) actor = *g_player;

		Utility::StopIdle(actor);
	}
}