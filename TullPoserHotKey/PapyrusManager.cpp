#include "Global.h"

namespace PapyrusManager {
	void ShowPoseMenu(StaticFunctionTag*) {
		ScaleformManager::TullPoserHotKeyMenu::OpenMenu("Pose");
	}

	void ShowExpressionMenu(StaticFunctionTag*) {
		InputEnableManager::SetInputEnableLayer();
		ScaleformManager::TullPoserHotKeyMenu::OpenMenu("Expression");
	}

	void StopIdle(StaticFunctionTag*) {
		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor) actor = *g_player;

		Utility::StopIdle(actor);
	}

	void RegisterPapyrusFuncs(VirtualMachine* vm) {
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ShowPoseMenu", "TullPoserHotKey", ShowPoseMenu, vm));
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ShowExpressionMenu", "TullPoserHotKey", ShowExpressionMenu, vm));
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("StopIdle", "TullPoserHotKey", StopIdle, vm));
	}
}