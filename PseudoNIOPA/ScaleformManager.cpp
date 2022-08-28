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

	std::string KeyCodeToControlID(UInt32 keyCode) {
		switch (keyCode) {
		case InputMap::kGamepadButtonOffset_DPAD_UP:
		case 0x26:	// Up Arrow
			return "Forward";
			break;

		case InputMap::kGamepadButtonOffset_DPAD_DOWN:
		case 0x28:	// Down Arrow
			return "Back";
			break;

		case InputMap::kGamepadButtonOffset_DPAD_LEFT:
		case 0x25:	// Left Arrow
			return "StrafeLeft";
			break;

		case InputMap::kGamepadButtonOffset_DPAD_RIGHT:
		case 0x27:	// Right Arrow
			return "StrafeRight";
			break;

		case InputMap::kGamepadButtonOffset_A:
		case 0x0D:	// Enter
			return "Activate";
			break;

		case InputMap::kGamepadButtonOffset_B:
		case 0x09:	// Tab
			return "Pipboy";
			break;
		}

		return std::string();
	}

	std::string DirectionToControlID(UInt32 dir) {
		switch (dir) {
		case 1:	// Up
			return "Forward";
			break;

		case 3:	// Down
			return "Back";
			break;

		case 4:	// Left
			return "StrafeLeft";
			break;

		case 2:	// Right
			return "StrafeRight";
			break;
		}

		return std::string();
	}

	bool IsControlIDDirection(std::string ctrlId) {
		if (ctrlId == "Forward" || ctrlId == "Back" ||
			ctrlId == "StrafeLeft" || ctrlId == "StrafeRight")
			return true;
		return false;
	}

	void SendKeyEvent(std::string ctrlId) {
		GFxMovieRoot* movieRoot = g_pnopMenu->movie->movieRoot;
		GFxValue root;

		if (!movieRoot->GetVariable(&root, "root")) {
			_ERROR("Couldn't get a root");
			return;
		}

		GFxValue ctrlName;
		ctrlName.SetString(ctrlId.c_str());

		root.Invoke("ProcessKeyEvent", nullptr, &ctrlName, 1);
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

		BSFixedString* control = inputEvent->GetControlID();
		std::string ctrlStr = control->c_str();

		if (ctrlStr == "Unmapped" || ctrlStr == "DISABLED") {
			std::string nCtrlStr = KeyCodeToControlID(keyCode);
			if (nCtrlStr.empty())
				return;
			ctrlStr = nCtrlStr;
		}
		else if (inputEvent->deviceType == InputEvent::kDeviceType_Gamepad) {
			std::string nCtrlStr = KeyCodeToControlID(keyCode);
			if (nCtrlStr.empty())
				return;
			ctrlStr = nCtrlStr;
		}

		bool isDown = inputEvent->isDown == 1.0f;
		bool isUp = inputEvent->isDown == 0.0f;

		if (isDown) {
			if (g_dirKeyDelayMap.find(keyCode) == g_dirKeyDelayMap.end())
				g_dirKeyDelayMap.insert(std::make_pair(keyCode, 0));
		}
		else {
			if (g_dirKeyDelayMap.find(keyCode) != g_dirKeyDelayMap.end())
				g_dirKeyDelayMap.erase(keyCode);
		}

		if (IsControlIDDirection(ctrlStr)) {
			if (isDown) {
				auto it = g_dirKeyDelayMap.find(keyCode);
				if (it != g_dirKeyDelayMap.end()) {
					if (it->second != 0) {
						it->second--;
						return;
					}
					else {
						if (inputEvent->timer == 0.0f)
							it->second = 25;
						else
							it->second = 3;
					}
				}

				SendKeyEvent(ctrlStr);
			}
		}
		else {
			if (isUp)
				SendKeyEvent(ctrlStr);
		}
	}

	void MenuInputHandler::OnThumbstickEvent(ThumbstickEvent* inputEvent) {
		static bool firstMove = false;
		static UInt64 delay = 0;

		if (inputEvent->deviceType != InputEvent::kDeviceType_Gamepad)
			return;

		BSFixedString* control = inputEvent->GetControlID();
		if (strcmp(control->c_str(), "Move") != 0)
			return;

		if (inputEvent->unk20[5] == 0) {
			firstMove = false;
			delay = 0;
			return;
		}

		if (delay != 0) {
			delay--;
			return;
		}
		else {
			if (!firstMove) {
				firstMove = true;
				delay = 25;
			}
			else {
				delay = 3;
			}
		}

		std::string ctrlStr = DirectionToControlID(inputEvent->unk20[5]);
		if (ctrlStr.empty())
			return;

		SendKeyEvent(ctrlStr);
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
		Actor* currActor = Utility::GetCurrentConsoleActor();
		bool sameActor = currActor && g_selectedActor == currActor;

		if (sameActor && !g_selectedMenu.empty())
			params[0].SetString(g_selectedMenu.c_str());
		else
			params[0].SetNull();

		if (sameActor && !g_selectedGroup.empty())
			params[1].SetString(g_selectedGroup.c_str());
		else
			params[1].SetNull();

		if (sameActor && g_selectedNode)
			params[2].SetString(g_selectedNode->m_name.c_str());
		else
			params[2].SetNull();

		root.Invoke("ShowMainMenu", nullptr, params, 3);
	}

	void CloseHandler::Invoke(Args* args) {
		bool closeAll = args->args[0].GetBool();
		if (!closeAll)
			g_selectedMenu = std::string();

		RegisterMenuInputHandler(false);
		PNIOPAMenu::CloseMenu();
	}

	void AllowTextInputHandler::Invoke(Args* args) {
		bool allowInput = args->args[0].GetBool();
		g_menuAllowTextInput = allowInput;
		(*g_inputMgr)->AllowTextInput(allowInput);
	}

	void ToggleFreezeHandler::Invoke(Args* args) {
		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor)
			return;

		if (!Utility::IsActorFrozen(actor))
			Nodes::InsertModMap(actor);
		Utility::ToggleFreeze(actor);

		g_selectedMenu = std::string();
	}

	void ResetActorHandler::Invoke(Args* args) {
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

		if (g_selectedActor == actor) {
			g_selectedActor = nullptr;
			g_selectedNode = nullptr;
		}

		g_selectedMenu = std::string();
	}

	void SelectNodeHandler::Invoke(Args* args) {
		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor)
			return;

		std::string nodeName = args->args[0].GetString();
		if (!actor || nodeName.empty())
			return;

		g_selectedNode = Nodes::GetNode(actor, nodeName);
		if (!g_selectedNode)
			return;

		g_selectedActor = actor;
	}

	void GetNodeListHandler::Invoke(Args* args) {
		args->movie->movieRoot->CreateArray(args->result);
		std::string groupName = args->args[0].GetString();
		if (groupName.empty())
			return;

		g_selectedMenu = "SELECTNODE";
		g_selectedGroup = groupName;

		std::vector<std::string> nodeList = ConfigReader::GetNodeListFromFile(std::string(groupName));
		for (const std::string& node : nodeList) {
			GFxValue nodeName;
			nodeName.SetString(node.c_str());

			args->result->PushBack(&nodeName);
		}
	}

	void GetNodeNameForSearchHandler::Invoke(Args* args) {
		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor) {
			args->result->SetNull();
			return;
		}

		if (!g_selectedNode || g_selectedActor != actor) {
			NiNode* rootNode = actor->GetActorRootNode(false);
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

		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor) {
			args->result->SetNull();
			return;
		}

		NiNode* node = Nodes::GetNode(actor, nodeName);
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

		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor)
			return;

		NiNode* node = Nodes::GetNode(actor, nodeName);
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

		std::string directory = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "\\Saves\\";
		const char* match = "*.pnop";

		for (IDirectoryIterator iter(directory.c_str(), match); !iter.Done(); iter.Next()) {
			WIN32_FIND_DATA* fileData = iter.Get();

			std::string s_fileName = fileData->cFileName;
			size_t lastDotIdx = s_fileName.find_last_of(".");
			std::string rawName = s_fileName.substr(0, lastDotIdx);

			GFxValue fileName;
			fileName.SetString(rawName.c_str());

			args->result->PushBack(&fileName);
		}
	}

	void SaveHandler::Invoke(Args* args) {
		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor) {
			args->result->SetString("ENOACTOR");
			return;
		}

		std::string saveName = args->args[0].GetString();
		if (saveName.empty()) {
			args->result->SetString("ESAVENAME");
			return;
		}

		auto modMapIt = g_modifiedMap.find(actor->formID);
		if (modMapIt == g_modifiedMap.end() || modMapIt->second.empty()) {
			args->result->SetString("ENOMOD");
			return;
		}

		if (!ConfigReader::SaveNodeData(saveName, actor, modMapIt->second)) {
			args->result->SetString("ENOPATH");
			return;
		}

		args->result->SetString("");
	}

	void LoadSaveHandler::Invoke(Args* args) {
		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor) {
			args->result->SetString("ENOACTOR");
			return;
		}

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

		Nodes::SetNodes(actor, nodeDataMap);

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