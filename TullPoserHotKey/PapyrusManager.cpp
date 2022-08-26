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

	void RegisterPapyrusFuncs(VirtualMachine* vm) {
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ShowMenu", "TullPoserHotKey", ShowMenu, vm));
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("StopIdle", "TullPoserHotKey", StopIdle, vm));
	}
}