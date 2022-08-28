#include "Global.h"

namespace PapyrusFuncs {
	bool IsActorFrozen(StaticFunctionTag*, Actor* actor) {
		if (!actor)
			return false;

		return Utility::IsActorFrozen(actor);
	}

	void ShowMenu(StaticFunctionTag*) {
		ScaleformManager::PNIOPAMenu::OpenMenu();
	}

	void ClearNodeSelection(StaticFunctionTag*) {
		Nodes::ClearSelect();
	}

	void ResetNode(StaticFunctionTag*) {
		Nodes::ResetSelectedNode();
	}

	void Register(VirtualMachine* vm) {
		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, Actor*>("IsActorFrozen", "PNIOPA_F4SE", IsActorFrozen, vm));
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ShowMenu", "PNIOPA_F4SE", ShowMenu, vm));
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ClearNodeSelection", "PNIOPA_F4SE", ClearNodeSelection, vm));
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ResetNode", "PNIOPA_F4SE", ResetNode, vm));
	}
};