#include "Global.h"

namespace Scaleform {
	const std::string PositionerMenuName = "AAFDynamicPositionerMenu";
	PositionerMenu* g_posMenu;
	bool PositionerMenu::isMenuOpen = false;
	std::map<BSInputEventUser*, bool> g_menuEnableMap;

	float g_offX, g_offY, g_offZ;

	MenuInputHandler g_posMenuInputHandler;

	void RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root) {
		RegisterFunction<MCMFunctionHandler>(f4se_root, view->movieRoot, "UpdateSettings");
		RegisterFunction<InitializeHandler>(f4se_root, view->movieRoot, "Initialize");
		RegisterFunction<CloseHandler>(f4se_root, view->movieRoot, "Close");
		RegisterFunction<SetPositionHandler>(f4se_root, view->movieRoot, "SetPosition");
	}

	void RegisterMenuInputHandler(bool bReg) {
		if (bReg) {
			g_posMenuInputHandler.enabled = true;
			tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
			BSInputEventUser* inputHandler = &g_posMenuInputHandler;
			int idx = inputEvents->GetItemIndex(inputHandler);
			if (idx == -1) {
				inputEvents->Push(&g_posMenuInputHandler);
				_MESSAGE("Menu Input Handler Registered");
			}
		}
		else {
			g_posMenuInputHandler.enabled = false;
		}
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

		// Prevent Arrow Keys
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
			return 0xFF;
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
		if (!PositionerMenu::IsMenuOpen())
			return;

		GFxMovieRoot* movieRoot = g_posMenu->movie->movieRoot;
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
		if (!PositionerMenu::IsMenuOpen())
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

		keyCode = ReplaceKeyCodeForMenu(keyCode);

		bool isDown = inputEvent->isDown == 1.0f;

		SendKeyEvent(keyCode, isDown);
	}

	void MenuInputHandler::OnThumbstickEvent(ThumbstickEvent* inputEvent) {
		if (!PositionerMenu::IsMenuOpen())
			return;

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

	PositionerMenu::PositionerMenu() {
		flags = kFlag_UsesCursor | kFlag_TopmostRenderedMenu;
		CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, movie, PositionerMenuName.c_str(), "root1", 4);
	}

	IMenu* PositionerMenu::CreateMenu() {
		return (g_posMenu = new PositionerMenu());
	}

	void PositionerMenu::RegisterMenu() {
		static BSFixedString menuName(PositionerMenuName.c_str());
		if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName))
			(*g_ui)->Register(menuName.c_str(), CreateMenu);

		_MESSAGE("Menu Registered");
	}

	void PositionerMenu::OpenMenu(float offX, float offY, float offZ) {
		g_offX = offX;
		g_offY = offY;
		g_offZ = offZ;

		isMenuOpen = true;
		Utility::BlockPlayerControls(true);
		Utility::EnableMenuControls(&g_posMenuInputHandler, g_menuEnableMap, false);
		InputEnable::SetInputEnableLayer();

		static BSFixedString menuName(PositionerMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
	}

	void PositionerMenu::UpdateMenu(float offX, float offY, float offZ) {
		if (!IsMenuOpen())
			return;

		g_offX = offX;
		g_offY = offY;
		g_offZ = offZ;

		GFxMovieRoot* movieRoot = g_posMenu->movie->movieRoot;
		GFxValue root;

		if (!movieRoot->GetVariable(&root, "root")) {
			_ERROR("Couldn't get a root");
			return;
		}

		GFxValue offset[3];
		offset[0].SetNumber(g_offX);
		offset[1].SetNumber(g_offY);
		offset[2].SetNumber(g_offZ);

		root.Invoke("UpdateOffset", nullptr, offset, 3);
	}

	void PositionerMenu::CloseMenu() {
		isMenuOpen = false;

		Utility::BlockPlayerControls(false);
		Utility::EnableMenuControls(&g_posMenuInputHandler, g_menuEnableMap, true);
		InputEnable::ResetInputEnableLayer();

		static BSFixedString menuName(PositionerMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
	}

	bool PositionerMenu::IsMenuOpen() {
		return isMenuOpen;
	}

	void MCMFunctionHandler::Invoke(Args* args) {
		if (args->numArgs == 0 || !args->args[0].IsString())
			return;

		if (args->numArgs == 2) {
			if (strcmp(args->args[0].GetString(), "bSeparatePlayerOffset") == 0)
				g_pluginSettings.bSeparatePlayerOffset = args->args[1].GetBool();
			else if (strcmp(args->args[0].GetString(), "bUnifyAAFDoppelgangerScale") == 0)
				g_pluginSettings.bUnifyAAFDoppelgangerScale = args->args[1].GetBool();
			else if (strcmp(args->args[0].GetString(), "fMoveAxisSize") == 0)
				g_pluginSettings.fMoveAxisSize = args->args[1].GetNumber();
			else if (strcmp(args->args[0].GetString(), "iPlayerPositionerType") == 0)
				g_pluginSettings.iPlayerPositionerType = (PositionerType)args->args[1].GetInt();
			else if (strcmp(args->args[0].GetString(), "iNPCPositionerType") == 0)
				g_pluginSettings.iNPCPositionerType = (PositionerType)args->args[1].GetInt();
		}
	}

	void InitializeHandler::Invoke(Args* args) {
		RegisterMenuInputHandler(true);

		GFxMovieRoot* movieRoot = args->movie->movieRoot;
		GFxValue root;

		if (!movieRoot->GetVariable(&root, "root")) {
			_ERROR("Couldn't get a root");
			return;
		}

		GFxValue offset[3];
		offset[0].SetNumber(g_offX);
		offset[1].SetNumber(g_offY);
		offset[2].SetNumber(g_offZ);

		root.Invoke("ShowMenu", nullptr, offset, 3);
	}

	void CloseHandler::Invoke(Args* args) {
		RegisterMenuInputHandler(false);
		PositionerMenu::CloseMenu();
	}

	void SetPositionHandler::Invoke(Args* args) {
		std::string axis = args->args[0].GetString();
		float value = args->args[1].GetNumber();

		Positioner::SetOffset(axis, value);
	}
}
