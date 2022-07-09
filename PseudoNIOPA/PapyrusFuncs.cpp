#include "Global.h"

namespace PapyrusFuncs {
	bool IsActorFrozen(StaticFunctionTag*, Actor* actor) {
		if (!actor)
			return false;

		return Utility::IsActorFrozen(actor);
	}

	void SelectNode(StaticFunctionTag*, Actor* actor, BSFixedString nodeName) {
		if (!actor || nodeName == "")
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

	void ClearButtons(StaticFunctionTag*, BGSMessage* msg) {
		if (!msg)
			return;

		MESSAGEBOX_BUTTON* ptr;
		while ((ptr = msg->buttonList.Pop())) {
			ptr->text.Release();
			Heap_Free(ptr);
		}
	}

	VMArray<BSFixedString> LoadNodes(StaticFunctionTag*, BSFixedString groupName) {
		VMArray<BSFixedString> retArr;
		std::vector<std::string> nodeList = ConfigReader::GetNodeListFromFile(std::string(groupName));

		for (std::string elem : nodeList) {
			BSFixedString nodeName(elem.c_str());
			retArr.Push(&nodeName);
			nodeName.Release();
		}

		return retArr;
	}

	bool AddButton(StaticFunctionTag*, BGSMessage* msg, BSFixedString btnStr) {
		if (!msg)
			return false;

		MESSAGEBOX_BUTTON* btn = (MESSAGEBOX_BUTTON*)Heap_Allocate(sizeof(MESSAGEBOX_BUTTON));
		if (!btn)
			return false;

		btn->text = btnStr;
		btn->conditions = 0;

		msg->buttonList.Insert(btn);

		return true;
	}

	void Register(VirtualMachine* vm) {
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, bool, Actor*>("IsActorFrozen", "PNIOPA_F4SE", IsActorFrozen, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, Actor*>("ToggleFreeze", "PNIOPA_F4SE", ToggleFreeze, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, Actor*>("ResetActor", "PNIOPA_F4SE", ResetActor, vm));
		vm->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, Actor*, BSFixedString>("SelectNode", "PNIOPA_F4SE", SelectNode, vm));
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, UInt32>("GetMenuKey", "PNIOPA_F4SE", GetMenuKey, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, BGSMessage*>("ClearButtons", "PNIOPA_F4SE", ClearButtons, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, VMArray<BSFixedString>, BSFixedString>("LoadNodes", "PNIOPA_F4SE", LoadNodes, vm));
		vm->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, bool, BGSMessage*, BSFixedString>("AddButton", "PNIOPA_F4SE", AddButton, vm));
	}
};