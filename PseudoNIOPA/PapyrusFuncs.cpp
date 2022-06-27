#include "Global.h"

namespace PapyrusFuncs {
	void SelectNode(StaticFunctionTag*, Actor* actor, BSFixedString nodeName) {
		if (!actor || nodeName == BSFixedString(""))
			return;

		NiNode* node = Nodes::GetNode(actor, nodeName);
		if (!node)
			return;

		g_selectedActor = actor;
		g_selectedNode = node;
	}

	void ToggleFreeze(StaticFunctionTag*, Actor *actor) {
		if (!Utility::IsActorFrozen(actor))
			Nodes::InsertModMap(actor);
		Utility::ToggleFreeze(actor);
	}

	void ResetActor(StaticFunctionTag*, Actor *actor) {
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

	UInt32 GetMenuKey(StaticFunctionTag*) {
		return g_pluginSettings.ShowMenuKey;
	}

	void Register(VirtualMachine* vm) {
		vm->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, Actor*, BSFixedString>("SelectNode", "PNIOPA_F4SE", SelectNode, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, Actor*>("ToggleFreeze", "PNIOPA_F4SE", ToggleFreeze, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, Actor*>("ResetActor", "PNIOPA_F4SE", ResetActor, vm));
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, UInt32>("GetMenuKey", "PNIOPA_F4SE", GetMenuKey, vm));
	}
};