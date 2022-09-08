#include "Global.h"

namespace PapyrusFuncs {
	bool IsActorFrozen(StaticFunctionTag*, Actor* actor) {
		if (!actor)
			return false;

		return Utility::IsActorFrozen(actor);
	}

	void ShowMenuNative(StaticFunctionTag*) {
		ScaleformManager::PNIOPAMenu::OpenMenu();
	}

	void ClearNodeSelection(StaticFunctionTag*) {
		Nodes::ClearSelect();
	}

	void ResetNode(StaticFunctionTag*) {
		Nodes::ResetSelectedNode();
	}

	void Register(VirtualMachine* vm) {
		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, Actor*>("IsActorFrozen", "PNIOPA", IsActorFrozen, vm));
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ShowMenuNative", "PNIOPA", ShowMenuNative, vm));
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ClearNodeSelection", "PNIOPA", ClearNodeSelection, vm));
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ResetNode", "PNIOPA", ResetNode, vm));
	}
};