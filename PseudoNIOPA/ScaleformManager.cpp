#include "Global.h"

namespace ScaleformManager {
	const std::string PNIOPAMenuName = "PNIOPAMenu";
	PNIOPAMenu* g_pnopMenu;
	bool g_menuAllowTextInput = false;

	std::string g_selectedMenu;
	std::string g_selectedGroup;

	MenuInputHandler g_menuInputHandler;
	std::map<UInt32, UInt64> g_dirKeyDelayMap;

	void RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root) {
		RegisterFunction<ScaleformManager::InitializeHandler>(f4se_root, view->movieRoot, "Initialize");
		RegisterFunction<ScaleformManager::CloseHandler>(f4se_root, view->movieRoot, "Close");
		RegisterFunction<ScaleformManager::AllowTextInputHandler>(f4se_root, view->movieRoot, "AllowTextInput");
		RegisterFunction<ScaleformManager::ToggleFreezeHandler>(f4se_root, view->movieRoot, "ToggleFreeze");
		RegisterFunction<ScaleformManager::ResetActorHandler>(f4se_root, view->movieRoot, "ResetActor");
		RegisterFunction<ScaleformManager::SelectNodeHandler>(f4se_root, view->movieRoot, "SelectNode");
		RegisterFunction<ScaleformManager::GetNodeListHandler>(f4se_root, view->movieRoot, "GetNodeList");
		RegisterFunction<ScaleformManager::GetNodeNameForSearchHandler>(f4se_root, view->movieRoot, "GetNodeNameForSearch");
		RegisterFunction<ScaleformManager::GetParentNodeNameHandler>(f4se_root, view->movieRoot, "GetParentNodeName");
		RegisterFunction<ScaleformManager::GetChildNodesHandler>(f4se_root, view->movieRoot, "GetChildNodes");
		RegisterFunction<ScaleformManager::GetSavesHandler>(f4se_root, view->movieRoot, "GetSaves");
		RegisterFunction<ScaleformManager::SaveHandler>(f4se_root, view->movieRoot, "Save");
		RegisterFunction<ScaleformManager::LoadSaveHandler>(f4se_root, view->movieRoot, "LoadSave");
		RegisterFunction<ScaleformManager::DeleteSaveHandler>(f4se_root, view->movieRoot, "DeleteSave");
	}

	void ClearMenuSelection() {
		g_selectedMenu = std::string();
		g_selectedGroup = std::string();
	}

	void RegisterMenuInputHandler(bool bReg) {
		if (bReg) {
			g_menuInputHandler.enabled = true;
			tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
			BSInputEventUser* inputHandler = &g_menuInputHandler;
			int idx = inputEvents->GetItemIndex(inputHandler);
			if (idx == -1) {
				inputEvents->Push(&g_menuInputHandler);
				_MESSAGE("Menu Input Handler Registered");
			}
		}
		else {
			g_menuInputHandler.enabled = false;
		}
	}

	void CloseMenu() {
		RegisterMenuInputHandler(false);
		PNIOPAMenu::CloseMenu();
	}

	UInt32 ReplaceKeyCodeForMenu(UInt32 keyCode) {
		switch (keyCode) {
		case InputMap::kGamepadButtonOffset_DPAD_UP:
		case 0x57:	// W Key
			return 0x26;
			break;

		case InputMap::kGamepadButtonOffset_DPAD_DOWN:
		case 0x53:	// S Key
			return 0x28;
			break;

		case InputMap::kGamepadButtonOffset_DPAD_LEFT:
		case 0x41:	// A Key
			return 0x25;
			break;

		case InputMap::kGamepadButtonOffset_DPAD_RIGHT:
		case 0x44:	// D Key
			return 0x27;
			break;

		case InputMap::kGamepadButtonOffset_A:
		case 0x45:	// E Key
			return 0x0D;
			break;

		case InputMap::kGamepadButtonOffset_B:
			return 0x09;
			break;
		}

		return keyCode;
	}

	UInt32 DirectionToKeyCode(UInt32 dir) {
		switch (dir) {
		case 1:	// Up
			return 0x26;
			break;

		case 3:	// Down
			return 0x28;
			break;

		case 4:	// Left
			return 0x25;
			break;

		case 2:	// Right
			return 0x27;
			break;
		}

		return 0xFF;
	}

	void SendKeyEvent(UInt32 keyCode, bool isDown) {
		GFxMovieRoot* movieRoot = g_pnopMenu->movie->movieRoot;
		GFxValue root;

		if (!movieRoot->GetVariable(&root, "root")) {
			_ERROR("Couldn't get a root");
			return;
		}

		GFxValue params[2];
		params[0].SetUInt(keyCode);
		params[1].SetBool(isDown);

		root.Invoke("ProcessKeyEvent", nullptr, params, 2);
	}

	void MenuInputHandler::OnButtonEvent(ButtonEvent* inputEvent) {
		if (!g_pnopMenu)
			return;

		if (g_menuAllowTextInput)
			return;

		UInt32 keyCode;

		// Mouse
		if (inputEvent->deviceType == InputEvent::kDeviceType_Mouse)
			keyCode = InputMap::kMacro_MouseButtonOffset + inputEvent->keyMask;
		// Gamepad
		else if (inputEvent->deviceType == InputEvent::kDeviceType_Gamepad)
			keyCode = InputMap::GamepadMaskToKeycode(inputEvent->keyMask);
		// Keyboard
		else
			keyCode = inputEvent->keyMask;

		// Valid scancode?
		if (keyCode >= InputMap::kMaxMacros)
			return;

		bool isDown = inputEvent->isDown == 1.0f;

		if (keyCode == g_pluginSettings.CloseMenukey && !isDown) {
			g_selectedMenu = std::string();
			CloseMenu();
			return;
		}

		keyCode = ReplaceKeyCodeForMenu(keyCode);

		SendKeyEvent(keyCode, isDown);
	}

	void MenuInputHandler::OnThumbstickEvent(ThumbstickEvent* inputEvent) {
		if (inputEvent->unk20[0] != 0xB)
			return;

		UInt32 prevKeyCode = DirectionToKeyCode(inputEvent->unk20[4]);
		UInt32 currKeyCode = DirectionToKeyCode(inputEvent->unk20[5]);
		if (currKeyCode != 0xFF) {
			SendKeyEvent(currKeyCode, true);
		}
		else {
			if (currKeyCode == prevKeyCode) {
				SendKeyEvent(0x25, false);
				SendKeyEvent(0x26, false);
				SendKeyEvent(0x27, false);
				SendKeyEvent(0x28, false);
			}
			else {
				SendKeyEvent(prevKeyCode, false);
			}
		}
	}

	PNIOPAMenu::PNIOPAMenu() {
		flags = kFlag_UsesCursor | kFlag_PausesGame;
		CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, movie, PNIOPAMenuName.c_str(), "root1", 4);
	}

	IMenu* PNIOPAMenu::CreateMenu() {
		return (g_pnopMenu = new PNIOPAMenu());
	}

	void PNIOPAMenu::RegisterMenu() {
		static BSFixedString menuName(PNIOPAMenuName.c_str());
		if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName))
			(*g_ui)->Register(menuName.c_str(), CreateMenu);

		_MESSAGE("Menu Registered");
	}

	void PNIOPAMenu::OpenMenu() {
		g_dirKeyDelayMap.clear();
		static BSFixedString menuName(PNIOPAMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
	}

	void PNIOPAMenu::CloseMenu() {
		static BSFixedString menuName(PNIOPAMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
	}

	void InitializeHandler::Invoke(Args* args) {
		RegisterMenuInputHandler(true);

		GFxMovieRoot* movieRoot = args->movie->movieRoot;
		GFxValue root;

		if (!movieRoot->GetVariable(&root, "root")) {
			_ERROR("Couldn't get a root");
			return;
		}

		GFxValue params[3];
		params[0].SetNull();
		params[1].SetNull();
		params[2].SetNull();

		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor)
			return;

		if (!g_selectedActor || g_selectedActor != actor) {
			g_selectedActor = actor;
			g_selectedNode = nullptr;
		}
		else {
			if (!g_selectedMenu.empty())
				params[0].SetString(g_selectedMenu.c_str());
			if (!g_selectedGroup.empty())
				params[1].SetString(g_selectedGroup.c_str());
			if (g_selectedNode)
				params[2].SetString(g_selectedNode->m_name.c_str());
		}

		root.Invoke("ShowMainMenu", nullptr, params, 3);
	}

	void CloseHandler::Invoke(Args* args) {
		bool closeAll = args->args[0].GetBool();
		if (!closeAll)
			g_selectedMenu = std::string();

		CloseMenu();
	}

	void AllowTextInputHandler::Invoke(Args* args) {
		bool allowInput = args->args[0].GetBool();
		g_menuAllowTextInput = allowInput;
		(*g_inputMgr)->AllowTextInput(allowInput);
	}

	void ToggleFreezeHandler::Invoke(Args* args) {
		if (!Utility::IsActorFrozen(g_selectedActor))
			Nodes::InsertModMap(g_selectedActor);
		Utility::ToggleFreeze(g_selectedActor);

		g_selectedMenu = std::string();
	}

	void ResetActorHandler::Invoke(Args* args) {
		auto it = g_modifiedMap.find(g_selectedActor->formID);
		if (it == g_modifiedMap.end()) {
			if (Utility::IsActorFrozen(g_selectedActor))
				Utility::UnfreezeActor(g_selectedActor);
			return;
		}

		for (auto sit : it->second)
			Nodes::ResetNode(g_selectedActor, sit.first, &sit.second);

		Utility::UnfreezeActor(g_selectedActor);

		g_selectedNode = nullptr;
		g_selectedMenu = std::string();
	}

	void SelectNodeHandler::Invoke(Args* args) {
		std::string nodeName = args->args[0].GetString();
		if (nodeName.empty())
			return;

		g_selectedNode = Nodes::GetNode(g_selectedActor, nodeName);
	}

	void GetNodeListHandler::Invoke(Args* args) {
		args->movie->movieRoot->CreateArray(args->result);
		std::string groupName = args->args[0].GetString();
		if (groupName.empty())
			return;

		std::vector<std::string> nodeList = ConfigReader::GetNodeListFromFile(std::string(groupName));
		for (const std::string& node : nodeList) {
			GFxValue nodeName;
			nodeName.SetString(node.c_str());

			args->result->PushBack(&nodeName);
		}

		g_selectedMenu = "SELECTNODE";
		g_selectedGroup = groupName;
	}

	void GetNodeNameForSearchHandler::Invoke(Args* args) {
		if (!g_selectedNode) {
			NiNode* rootNode = g_selectedActor->GetActorRootNode(false);
			if (!rootNode) {
				args->result->SetNull();
				return;
			}

			args->result->SetString(rootNode->m_name.c_str());
		}
		else {
			args->result->SetString(g_selectedNode->m_name.c_str());
		}

		g_selectedMenu = "SEARCHNODE";
	}

	void GetParentNodeNameHandler::Invoke(Args* args) {
		std::string nodeName = args->args[0].GetString();
		if (nodeName.empty()) {
			args->result->SetNull();
			return;
		}

		NiNode* node = Nodes::GetNode(g_selectedActor, nodeName);
		if (!node) {
			args->result->SetNull();
			return;
		}

		if (!node->m_parent) {
			args->result->SetNull();
			return;
		}

		args->result->SetString(node->m_parent->m_name.c_str());
	}

	void GetChildNodesHandler::Invoke(Args* args) {
		args->movie->movieRoot->CreateArray(args->result);
		std::string nodeName = args->args[0].GetString();
		if (nodeName.empty())
			return;

		NiNode* node = Nodes::GetNode(g_selectedActor, nodeName);
		if (!node)
			return;

		for (UInt32 ii = 0; ii < node->m_children.m_size; ii++) {
			NiAVObject* childObject = node->m_children.m_data[ii];
			if (!childObject)
				continue;

			NiNode* childNode = childObject->GetAsNiNode();
			if (!childNode)
				continue;

			GFxValue childNodeName;
			childNodeName.SetString(childNode->m_name.c_str());

			args->result->PushBack(&childNodeName);
		}
	}

	void GetSavesHandler::Invoke(Args* args) {
		args->movie->movieRoot->CreateArray(args->result);

		std::vector<std::string> nodeDataList = ConfigReader::GetNodeDataList();

		for (std::string nodeDataName : nodeDataList) {
			GFxValue fileName;
			fileName.SetString(nodeDataName.c_str());

			args->result->PushBack(&fileName);
		}
	}

	void SaveHandler::Invoke(Args* args) {
		std::string saveName = args->args[0].GetString();
		if (saveName.empty()) {
			args->result->SetString("ESAVENAME");
			return;
		}

		auto modMapIt = g_modifiedMap.find(g_selectedActor->formID);
		if (modMapIt == g_modifiedMap.end() || modMapIt->second.empty()) {
			args->result->SetString("ENOMOD");
			return;
		}

		if (!ConfigReader::SaveNodeData(saveName, g_selectedActor, modMapIt->second)) {
			args->result->SetString("ENOPATH");
			return;
		}

		args->result->SetString("");
	}

	void LoadSaveHandler::Invoke(Args* args) {
		std::string saveName = args->args[0].GetString();
		if (saveName.empty()) {
			args->result->SetString("ESAVENAME");
			return;
		}

		std::unordered_map<std::string, Nodes::NodeData> nodeDataMap = ConfigReader::LoadNodeData(saveName);
		if (nodeDataMap.empty()) {
			args->result->SetString("ENOPATH");
			return;
		}

		Nodes::SetNodes(g_selectedActor, nodeDataMap);
		g_selectedMenu = std::string();

		args->result->SetString("");
	}

	void DeleteSaveHandler::Invoke(Args* args) {
		std::string saveName = args->args[0].GetString();
		if (saveName.empty()) {
			args->result->SetString("ESAVENAME");
			return;
		}

		if (!ConfigReader::DeleteNodeData(saveName)) {
			args->result->SetString("ENOPATH");
			return;
		}

		args->result->SetString("");
	}
}