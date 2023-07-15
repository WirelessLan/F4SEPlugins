#include "Global.h"

namespace PapyrusFuncs {
	bool IsActorFrozen(StaticFunctionTag*, Actor* a_actor) {
		if (!a_actor)
			return false;

		return Utility::IsActorFrozen(a_actor);
	}

	void ShowMenuNative(StaticFunctionTag*) {
		ScaleformManager::OpenMenu();
	}

	void ClearNodeSelection(StaticFunctionTag*) {
		Nodes::ClearSelect();
	}

	void ResetNode(StaticFunctionTag*) {
		Nodes::ResetSelectedNode();
	}

	void ToggleFreeze(StaticFunctionTag*) {
		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor)
			return;

		if (!Utility::IsActorFrozen(actor))
			Nodes::InsertModMap(actor);
		Utility::ToggleFreeze(actor);
	}

	void ResetActor(StaticFunctionTag*) {
		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor)
			return;

		auto it = g_modifiedMap.find(actor->formID);
		if (it == g_modifiedMap.end()) {
			if (Utility::IsActorFrozen(actor))
				Utility::UnfreezeActor(actor);
			return;
		}

		for (auto sit : it->second)
			Nodes::ResetNode(actor, sit.first, &sit.second);
		Utility::UnfreezeActor(actor);
	}

	void Register(VirtualMachine* a_vm) {
		a_vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, Actor*>("IsActorFrozen", "PNIOPA", IsActorFrozen, a_vm));
		a_vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ShowMenuNative", "PNIOPA", ShowMenuNative, a_vm));
		a_vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ClearNodeSelection", "PNIOPA", ClearNodeSelection, a_vm));
		a_vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ResetNode", "PNIOPA", ResetNode, a_vm));
		a_vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ToggleFreezeNative", "PNIOPA", ToggleFreeze, a_vm));
		a_vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ResetActorNative", "PNIOPA", ResetActor, a_vm));
	}
};