#include "Global.h"

std::string g_selectedPlugin;
std::string g_selectedPose;

namespace ScaleformManager {
	const std::string TullPoserHotKeyMenuName = "TullPoserHotKeyMenu";
	TullPoserHotKeyMenu* g_poserMenu;
	bool g_menuAllowTextInput = false;

	MenuInputHandler g_poserMenuInputHandler;
	std::map<UInt32, UInt64> g_dirKeyDelayMap;

	void RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root) {
		RegisterFunction<ScaleformManager::InitializeHandler>(f4se_root, view->movieRoot, "Initialize");
		RegisterFunction<ScaleformManager::CloseHandler>(f4se_root, view->movieRoot, "Close");
		RegisterFunction<ScaleformManager::GetPluginListHandler>(f4se_root, view->movieRoot, "GetPluginList");
		RegisterFunction<ScaleformManager::SelectPluginHandler>(f4se_root, view->movieRoot, "SelectPlugin");
		RegisterFunction<ScaleformManager::GetPoseListHandler>(f4se_root, view->movieRoot, "GetPoseList");
		RegisterFunction<ScaleformManager::SelectPoseHandler>(f4se_root, view->movieRoot, "SelectPose");
	}

	void ClearSelectedVars() {
		g_selectedPlugin.clear();
		g_selectedPose.clear();
	}

	void RegisterMenuInputHandler(bool bReg) {
		if (bReg) {
			g_poserMenuInputHandler.enabled = true;
			tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
			BSInputEventUser* inputHandler = &g_poserMenuInputHandler;
			int idx = inputEvents->GetItemIndex(inputHandler);
			if (idx == -1) {
				inputEvents->Push(&g_poserMenuInputHandler);
				_MESSAGE("Menu Input Handler Registered");
			}
		}
		else {
			g_poserMenuInputHandler.enabled = false;
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
		GFxMovieRoot* movieRoot = g_poserMenu->movie->movieRoot;
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
		if (!g_poserMenu)
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

	void MenuInputHandler::OnThumbstickEvent(ThumbstickEvent * inputEvent) {
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

	TullPoserHotKeyMenu::TullPoserHotKeyMenu() {
		flags = kFlag_UsesCursor | kFlag_PausesGame;
		CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, movie, TullPoserHotKeyMenuName.c_str(), "root1", 4);
	}

	IMenu* TullPoserHotKeyMenu::CreateMenu() {
		return (g_poserMenu = new TullPoserHotKeyMenu());
	}

	void TullPoserHotKeyMenu::RegisterMenu() {
		static BSFixedString menuName(TullPoserHotKeyMenuName.c_str());
		if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName))
			(*g_ui)->Register(menuName.c_str(), CreateMenu);

		_MESSAGE("Menu Registered");
	}

	void TullPoserHotKeyMenu::OpenMenu() {
		g_dirKeyDelayMap.clear();
		static BSFixedString menuName(TullPoserHotKeyMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
	}

	void TullPoserHotKeyMenu::CloseMenu() {
		static BSFixedString menuName(TullPoserHotKeyMenuName.c_str());
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

		GFxValue in_args[2];
		if (g_selectedPlugin.empty())
			in_args[0].SetNull();
		else 
			in_args[0].SetString(g_selectedPlugin.c_str());

		if (g_selectedPose.empty())
			in_args[1].SetNull();
		else
			in_args[1].SetString(g_selectedPose.c_str());

		root.Invoke("ShowMainMenu", nullptr, in_args, 2);
	}

	void CloseHandler::Invoke(Args* args) {
		RegisterMenuInputHandler(false);
		TullPoserHotKeyMenu::CloseMenu();
	}

	void GetPluginListHandler::Invoke(Args* args) {
		args->movie->movieRoot->CreateArray(args->result);

		for (const std::string& plugin : g_pluginVec) {
			GFxValue pluginName;
			pluginName.SetString(plugin.c_str());

			args->result->PushBack(&pluginName);
		}
	}

	void SelectPluginHandler::Invoke(Args* args) {
		if (args->numArgs == 1) {
			std::string pluginName = args->args[0].GetString();

			g_selectedPlugin = pluginName;
		}
	}

	void GetPoseListHandler::Invoke(Args* args) {
		args->movie->movieRoot->CreateArray(args->result);

		if (args->numArgs == 1) {
			std::string pluginName = args->args[0].GetString();

			std::vector<std::string> customPose = ConfigReader::ReadPluginPoseConfig(pluginName);
			if (!customPose.empty()) {
				for (std::string pose : customPose) {
					GFxValue poseName;
					poseName.SetString(pose.c_str());

					args->result->PushBack(&poseName);
				}
			}
			else {
				auto idleAnimMap_it = g_idleAnimMap.find(pluginName);
				if (idleAnimMap_it != g_idleAnimMap.end()) {
					for (auto idle : idleAnimMap_it->second) {
						const char* dyn_prefix = "dyn_";
						if (idle.second->animEventName && strncmp(dyn_prefix, idle.second->animEventName.c_str(), strlen(dyn_prefix)) != 0)
							continue;

						GFxValue poseName;
						poseName.SetString(idle.first.c_str());

						args->result->PushBack(&poseName);
					}
				}
			}
		}
	}

	void SelectPoseHandler::Invoke(Args* args) {
		if (args->numArgs == 2) {
			std::string pluginName = args->args[0].GetString();
			std::string poseName = args->args[1].GetString();

			g_selectedPlugin = pluginName;
			g_selectedPose = poseName;

			Actor* actor = Utility::GetCurrentConsoleActor();
			if (!actor) actor = *g_player;

			TESIdleForm* idle = Utility::GetIdleForm(g_selectedPlugin, g_selectedPose);
			if (!idle)
				return;

			Utility::PlayIdle(actor, idle);
		}
	}
}
