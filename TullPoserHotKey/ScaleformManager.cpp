#include "Global.h"

namespace ScaleformManager {
	const std::string TullPoserHotKeyMenuName = "TullPoserHotKeyMenu";
	TullPoserHotKeyMenu* g_poserMenu;
	bool g_menuAllowTextInput = false;

	std::string g_selectedPlugin;
	std::string g_selectedPose;

	MenuInputHandler g_poserMenuInputHandler;

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

	std::string KeyCodeToString(UInt32 keyCode) {
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

		case 0x0D:	// Enter
			return "Activate";
			break;
		}

		return std::string();
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

		if (ctrlStr == "Unmapped") {
			std::string nCtrlStr = KeyCodeToString(keyCode);
			ctrlStr = nCtrlStr.empty() ? ctrlStr : nCtrlStr;
			if (ctrlStr == "Unmapped")
				return;
		}

		bool isDown = inputEvent->isDown == 1.0f && inputEvent->timer == 0.0f;

		if (isDown) {
			GFxMovieRoot* movieRoot = g_poserMenu->movie->movieRoot;
			GFxValue root;

			if (!movieRoot->GetVariable(&root, "root")) {
				_ERROR("Couldn't get a root");
				return;
			}

			GFxValue ctrlName;
			ctrlName.SetString(ctrlStr.c_str());

			root.Invoke("ProcessKeyEvent", nullptr, &ctrlName, 1);
		}
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
		static BSFixedString menuName(TullPoserHotKeyMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
	}

	void TullPoserHotKeyMenu::CloseMenu() {
		static BSFixedString menuName(TullPoserHotKeyMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
	}

	void ClearSelectedVars() {
		g_selectedPlugin.clear();
		g_selectedPose.clear();
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
		if (args->numArgs == 1) {
			bool isCloseAll = args->args[0].GetBool();
			if (!isCloseAll)
				ClearSelectedVars();
		}

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

			auto plugin_it = g_idleAnimMap.find(pluginName);
			if (plugin_it == g_idleAnimMap.end()) {
				_MESSAGE("Cannot find a plugin: %s", pluginName.c_str());
				return;
			}

			auto pose_it = plugin_it->second.find(poseName);
			if (pose_it == plugin_it->second.end()) {
				_MESSAGE("Cannot find a Pose: %s", poseName.c_str());
				return;
			}
			
			Actor* actor = Utility::GetCurrentConsoleActor();
			if (!actor) actor = *g_player;

			Utility::PlayIdle(actor, pose_it->second);
		}
	}
}
