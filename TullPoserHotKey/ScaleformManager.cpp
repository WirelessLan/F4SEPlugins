#include "Global.h"

namespace ScaleformManager {
	const std::string TullPoserHotKeyMenuName = "TullPoserHotKeyMenu";
	TullPoserHotKeyMenu* g_poserMenu;
	std::string TullPoserHotKeyMenu::MenuType = std::string();
	bool g_menuAllowTextInput = false;

	std::string g_selectedPlugin;
	std::string g_selectedPose;
	Actor* g_selectedActor;

	MenuInputHandler g_poserMenuInputHandler;
	std::map<UInt32, UInt64> g_dirKeyDelayMap;

	void RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root) {
		RegisterFunction<ScaleformManager::InitializeHandler>(f4se_root, view->movieRoot, "Initialize");
		RegisterFunction<ScaleformManager::CloseHandler>(f4se_root, view->movieRoot, "Close");
		RegisterFunction<ScaleformManager::AllowTextInputHandler>(f4se_root, view->movieRoot, "AllowTextInput");
		RegisterFunction<ScaleformManager::GetPluginListHandler>(f4se_root, view->movieRoot, "GetPluginList");
		RegisterFunction<ScaleformManager::SelectPluginHandler>(f4se_root, view->movieRoot, "SelectPlugin");
		RegisterFunction<ScaleformManager::GetPoseListHandler>(f4se_root, view->movieRoot, "GetPoseList");
		RegisterFunction<ScaleformManager::SelectPoseHandler>(f4se_root, view->movieRoot, "SelectPose");
		RegisterFunction<ScaleformManager::SetExpressionHandler>(f4se_root, view->movieRoot, "SetExpression");
		RegisterFunction<ScaleformManager::ResetExpressionHandler>(f4se_root, view->movieRoot, "ResetExpression");
		RegisterFunction<ScaleformManager::SaveExpressionHandler>(f4se_root, view->movieRoot, "SaveExpression");
		RegisterFunction<ScaleformManager::GetExpressionsHandler>(f4se_root, view->movieRoot, "GetExpressions");
		RegisterFunction<ScaleformManager::LoadExpressionHandler>(f4se_root, view->movieRoot, "LoadExpression");
		RegisterFunction<ScaleformManager::DeleteExpressionHandler>(f4se_root, view->movieRoot, "DeleteExpression");
	}

	void ClearSelectedVars() {
		g_selectedPlugin.clear();
		g_selectedPose.clear();
		g_selectedActor = nullptr;
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
		GFxMovieRoot* movieRoot = g_poserMenu->movie->movieRoot;
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

		keyCode = ReplaceKeyCodeForMenu(keyCode);

		bool isDown = inputEvent->isDown == 1.0f;

		SendKeyEvent(keyCode, isDown);
	}

	void MenuInputHandler::OnThumbstickEvent(ThumbstickEvent * inputEvent) {
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

	TullPoserHotKeyMenu::TullPoserHotKeyMenu(UInt32 _flags) {
		flags = _flags;
		CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, movie, TullPoserHotKeyMenuName.c_str(), "root1", 4);
	}

	IMenu* TullPoserHotKeyMenu::CreateMenu() {
		UInt32 flags = 0;
		if (TullPoserHotKeyMenu::MenuType == "Pose")
			flags = kFlag_UsesCursor | kFlag_PausesGame;
		else if (TullPoserHotKeyMenu::MenuType == "Expression")
			flags = kFlag_UsesCursor;
		return (g_poserMenu = new TullPoserHotKeyMenu(flags));
	}

	void TullPoserHotKeyMenu::RegisterMenu() {
		static BSFixedString menuName(TullPoserHotKeyMenuName.c_str());
		if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName))
			(*g_ui)->Register(menuName.c_str(), CreateMenu);

		_MESSAGE("Menu Registered");
	}

	void TullPoserHotKeyMenu::OpenMenu(const std::string& menuType) {
		g_dirKeyDelayMap.clear();
		TullPoserHotKeyMenu::MenuType = menuType;
		static BSFixedString menuName(TullPoserHotKeyMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
	}

	void TullPoserHotKeyMenu::CloseMenu() {
		static BSFixedString menuName(TullPoserHotKeyMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
	}

	void AllowTextInputHandler::Invoke(Args* args) {
		bool allowInput = args->args[0].GetBool();
		g_menuAllowTextInput = allowInput;
		(*g_inputMgr)->AllowTextInput(allowInput);
	}

	void InitializeHandler::Invoke(Args* args) {
		RegisterMenuInputHandler(true);

		GFxMovieRoot* movieRoot = args->movie->movieRoot;
		GFxValue root;

		if (!movieRoot->GetVariable(&root, "root")) {
			_ERROR("Couldn't get a root");
			return;
		}

		Actor* actor = Utility::GetCurrentConsoleActor();
		if (!actor) actor = *g_player;

		g_selectedActor = actor;

		if (TullPoserHotKeyMenu::MenuType == "Pose") {
			GFxValue in_args[2];
			if (g_selectedPlugin.empty())
				in_args[0].SetNull();
			else
				in_args[0].SetString(g_selectedPlugin.c_str());

			if (g_selectedPose.empty())
				in_args[1].SetNull();
			else
				in_args[1].SetString(g_selectedPose.c_str());

			root.Invoke("ShowPoseMenu", nullptr, in_args, 2);
		}
		else if (TullPoserHotKeyMenu::MenuType == "Expression") {
			ExpressionManager::BSFaceGenAnimationData* fgAnimData = ExpressionManager::GetFaceGenAnimationData(g_selectedActor);
			if (!fgAnimData)
				return;

			GFxValue arr;
			movieRoot->CreateArray(&arr);
			for (UInt32 ii = 0; ii < 54; ii++) {
				GFxValue exp;
				exp.SetUInt(fgAnimData->exprs[ii] * 100);
				arr.PushBack(&exp);
			}

			root.Invoke("ShowExpressionMenu", nullptr, &arr, 1);
		}
	}

	void CloseHandler::Invoke(Args* args) {
		InputEnableManager::ResetInputEnableLayer();
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
		std::string pluginName = args->args[0].GetString();

		g_selectedPlugin = pluginName;
	}

	void GetPoseListHandler::Invoke(Args* args) {
		args->movie->movieRoot->CreateArray(args->result);

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

	void SelectPoseHandler::Invoke(Args* args) {
		std::string pluginName = args->args[0].GetString();
		std::string poseName = args->args[1].GetString();

		g_selectedPlugin = pluginName;
		g_selectedPose = poseName;

		TESIdleForm* idle = Utility::GetIdleForm(g_selectedPlugin, g_selectedPose);
		if (!idle)
			return;

		Utility::PlayIdle(g_selectedActor, idle);
	}

	void SetExpressionHandler::Invoke(Args* args) {
		UInt32 idx = args->args[0].GetUInt();
		UInt32 value = args->args[1].GetUInt();

		ExpressionManager::BSFaceGenAnimationData* fgAnimData = ExpressionManager::GetFaceGenAnimationData(g_selectedActor);
		if (!fgAnimData)
			return;

		ExpressionManager::SetFaceGenMorphs(g_selectedActor, idx, value / 100.0f);
	}

	void ResetExpressionHandler::Invoke(Args* args) {
		ExpressionManager::ResetExpression(g_selectedActor);
	}

	void SaveExpressionHandler::Invoke(Args* args)	{
		ExpressionManager::BSFaceGenAnimationData* fgAnimData = ExpressionManager::GetFaceGenAnimationData(g_selectedActor);
		if (!fgAnimData) {
			args->result->SetBool(false);
			return;
		}

		std::string saveName = args->args[0].GetString();
		if (saveName.empty()) {
			args->result->SetBool(false);
			return;
		}

		if (!ExpressionManager::SaveExpression(saveName, fgAnimData)) {
			args->result->SetBool(false);
			return;
		}

		args->result->SetBool(true);
	}

	void GetExpressionsHandler::Invoke(Args* args) {
		args->movie->movieRoot->CreateArray(args->result);

		std::vector<std::string> exps = ExpressionManager::GetExpressions();
		for (std::string exp : exps) {
			GFxValue fileName;
			fileName.SetString(exp.c_str());

			args->result->PushBack(&fileName);
		}
	}

	void LoadExpressionHandler::Invoke(Args* args)	{
		std::string saveName = args->args[0].GetString();
		if (saveName.empty()) {
			args->result->SetBool(false);
			return;
		}

		if (!ExpressionManager::LoadExpression(g_selectedActor, saveName)) {
			args->result->SetBool(false);
			return;
		}

		args->result->SetBool(true);
	}

	void DeleteExpressionHandler::Invoke(Args* args) {
		std::string saveName = args->args[0].GetString();
		if (saveName.empty()) {
			args->result->SetBool(false);
			return;
		}

		if (!ExpressionManager::DeleteExpression(saveName)) {
			args->result->SetBool(false);
			return;
		}

		args->result->SetBool(true);
	}
}
