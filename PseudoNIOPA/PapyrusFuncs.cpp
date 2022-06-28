#include "Global.h"

namespace PapyrusFuncs {
	bool IsActorFrozen(StaticFunctionTag*, Actor* actor) {
		if (!actor)
			return false;

		return Utility::IsActorFrozen(actor);
	}

	void SelectNode(StaticFunctionTag*, Actor* actor, BSFixedString groupName, UInt32 selection) {
		if (!actor || groupName == BSFixedString() || !selection)
			return;

		std::string nodeName = ConfigReader::GetNodeNameFromFile(std::string(groupName), selection);
		if (nodeName.empty())
			return;

		g_selectedNode = Nodes::GetNode(actor, nodeName);
		if (!g_selectedNode)
			return;
		g_selectedActor = actor;
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

		if (g_selectedActor == actor) {
			g_selectedActor = nullptr;
			g_selectedNode = nullptr;
		}
	}

	UInt32 GetMenuKey(StaticFunctionTag*) {
		return g_pluginSettings.ShowMenuKey;
	}

	void Register(VirtualMachine* vm) {
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, Actor*>("IsActorFrozen", "PNIOPA_F4SE", IsActorFrozen, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, Actor*>("ToggleFreeze", "PNIOPA_F4SE", ToggleFreeze, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, Actor*>("ResetActor", "PNIOPA_F4SE", ResetActor, vm));
		vm->RegisterFunction(
			new NativeFunction3<StaticFunctionTag, void, Actor*, BSFixedString, UInt32>("SelectNode", "PNIOPA_F4SE", SelectNode, vm));
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, UInt32>("GetMenuKey", "PNIOPA_F4SE", GetMenuKey, vm));
	}
};