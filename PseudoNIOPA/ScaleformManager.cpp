#include "Global.h"

namespace ScaleformManager {
	const std::string PNIOPAMenuName = "PNIOPAMenu";
	bool g_menuAllowTextInput = false;

	std::string g_selectedMenu;
	std::string g_selectedGroup;

	class PNIOPAMenu : public GameMenuBase {
	public:
		PNIOPAMenu() {
			this->Instance = this;
			this->flags = kFlag_UsesCursor | kFlag_PausesGame;
			this->enabled = false;

			if (*g_scaleformManager)
				CALL_MEMBER_FN(*g_scaleformManager, LoadMovie)(this, movie, PNIOPAMenuName.c_str(), "root1", 4);
		}

		~PNIOPAMenu() {
			Instance = nullptr;
		}

		virtual bool ShouldHandleEvent(InputEvent* a_inputEvent) override { return true; }

		virtual void OnButtonEvent(ButtonEvent* inputEvent) override;
		virtual void OnThumbstickEvent(ThumbstickEvent* inputEvent) override;

		static PNIOPAMenu* GetSingleton() {
			return Instance;
		}

	private:
		static PNIOPAMenu* Instance;
	};

	class Localizations {
	public:
		Localizations() {}

		static Localizations* GetSingleton() {
			static Localizations loc;
			return &loc;
		}

		std::string lang;
		std::unordered_map<std::string, std::string> translationsMap;
	};

	PNIOPAMenu* PNIOPAMenu::Instance = nullptr;

	std::uint32_t ReplaceKeyCodeForMenu(std::uint32_t a_keyCode) {
		switch (a_keyCode) {
		case InputMap::kGamepadButtonOffset_DPAD_UP:
		case 0x57:	// W Key
			return 0x26;

		case InputMap::kGamepadButtonOffset_DPAD_DOWN:
		case 0x53:	// S Key
			return 0x28;

		case InputMap::kGamepadButtonOffset_DPAD_LEFT:
		case 0x41:	// A Key
			return 0x25;

		case InputMap::kGamepadButtonOffset_DPAD_RIGHT:
		case 0x44:	// D Key
			return 0x27;

		case InputMap::kGamepadButtonOffset_A:
		case 0x45:	// E Key
			return 0x0D;

		case InputMap::kGamepadButtonOffset_B:
			return 0x09;
		}

		return a_keyCode;
	}

	std::uint32_t DirectionToKeyCode(std::uint32_t a_dir) {
		switch (a_dir) {
		case 1:	// Up
			return 0x26;

		case 3:	// Down
			return 0x28;

		case 4:	// Left
			return 0x25;

		case 2:	// Right
			return 0x27;
		}

		return 0xFF;
	}

	void SendKeyEvent(std::uint32_t a_keyCode, bool a_isDown) {
		PNIOPAMenu* g_pnopMenu = PNIOPAMenu::GetSingleton();
		if (!g_pnopMenu)
			return;

		GFxMovieRoot* movieRoot = g_pnopMenu->movie->movieRoot;
		GFxValue root;

		if (!movieRoot || !movieRoot->GetVariable(&root, "root")) {
			_ERROR("Couldn't get a root");
			return;
		}

		GFxValue params[2];
		params[0].SetUInt(a_keyCode);
		params[1].SetBool(a_isDown);

		root.Invoke("ProcessKeyEvent", nullptr, params, 2);
	}

	void PNIOPAMenu::OnButtonEvent(ButtonEvent* a_inputEvent) {
		if (g_menuAllowTextInput)
			return;

		std::uint32_t keyCode;

		// Mouse
		if (a_inputEvent->deviceType == InputEvent::kDeviceType_Mouse)
			keyCode = InputMap::kMacro_MouseButtonOffset + a_inputEvent->keyMask;
		// Gamepad
		else if (a_inputEvent->deviceType == InputEvent::kDeviceType_Gamepad)
			keyCode = InputMap::GamepadMaskToKeycode(a_inputEvent->keyMask);
		// Keyboard
		else
			keyCode = a_inputEvent->keyMask;

		// Valid scancode?
		if (keyCode >= InputMap::kMaxMacros)
			return;

		keyCode = ReplaceKeyCodeForMenu(keyCode);
		SendKeyEvent(keyCode, a_inputEvent->isDown == 1.0f);
	}

	void PNIOPAMenu::OnThumbstickEvent(ThumbstickEvent* a_inputEvent) {
		if (a_inputEvent->unk20[0] != 0xB)
			return;

		std::uint32_t prevKeyCode = DirectionToKeyCode(a_inputEvent->unk20[4]);
		std::uint32_t currKeyCode = DirectionToKeyCode(a_inputEvent->unk20[5]);
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

	class ThrowHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			if (a_params->numArgs > 0)
				_MESSAGE("Throw Message: %s", a_params->args[0].GetString());
			CloseMenu();
		}
	};

	class GetInitializationDataHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			PNIOPAMenu* g_pnopMenu = PNIOPAMenu::GetSingleton();
			if (!g_pnopMenu)
				return;

			GFxMovieRoot* movieRoot = a_params->movie->movieRoot;
			if (!movieRoot) {
				_MESSAGE("GetInitializationDataHandler: Couldn't get a movieRoot!");
				return;
			}

			Localizations* g_loc = Localizations::GetSingleton();
			if (!g_loc) {
				_MESSAGE("GetInitializationDataHandler: Couldn't get the localization data!");
				return;
			}

			movieRoot->CreateObject(a_params->result);

			GFxValue langVal;
			langVal.SetString(g_loc->lang.c_str());

			GFxValue locVal;
			movieRoot->CreateObject(&locVal);
			for (const auto& transPair : g_loc->translationsMap) {
				GFxValue val;
				val.SetString(transPair.second.c_str());
				locVal.SetMember(transPair.first.c_str(), &val);
			}

			a_params->result->SetMember("Language", &langVal);
			a_params->result->SetMember("Localizations", &locVal);
		}
	};

	class InitializationCompleteHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			PNIOPAMenu* g_pnopMenu = PNIOPAMenu::GetSingleton();
			if (!g_pnopMenu)
				return;

			g_pnopMenu->enabled = true;

			GFxMovieRoot* movieRoot = a_params->movie->movieRoot;
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
	};

	class CloseHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			if (a_params->numArgs > 0 && a_params->args[0].IsBool()) {
				bool closeAll = a_params->args[0].GetBool();
				if (!closeAll)
					g_selectedMenu = std::string();
			}

			CloseMenu();
		}
	};

	class AllowTextInputHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			bool allowInput = a_params->args[0].GetBool();
			g_menuAllowTextInput = allowInput;
			(*g_inputMgr)->AllowTextInput(allowInput);
		}
	};

	class ToggleFreezeHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			if (!Utility::IsActorFrozen(g_selectedActor))
				Nodes::InsertModMap(g_selectedActor);
			Utility::ToggleFreeze(g_selectedActor);

			ClearMenuSelection();
		}
	};

	class ResetActorHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
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

			ClearMenuSelection();
		}
	};

	class SelectNodeHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			std::string nodeName = a_params->args[0].GetString();
			if (nodeName.empty())
				return;

			g_selectedNode = Nodes::GetNode(g_selectedActor, nodeName);
		}
	};

	class GetNodeListHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			a_params->movie->movieRoot->CreateArray(a_params->result);
			std::string groupName = a_params->args[0].GetString();
			if (groupName.empty())
				return;

			std::vector<std::string> nodeList = ConfigReader::GetNodeListFromFile(std::string(groupName));
			for (const std::string& node : nodeList) {
				GFxValue nodeName;
				nodeName.SetString(node.c_str());

				a_params->result->PushBack(&nodeName);
			}

			g_selectedMenu = "SELECTNODE";
			g_selectedGroup = groupName;
		}
	};

	class GetNodeNameForSearchHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			if (!g_selectedNode) {
				NiNode* rootNode = g_selectedActor->GetActorRootNode(false);
				if (!rootNode) {
					a_params->result->SetNull();
					return;
				}

				a_params->result->SetString(rootNode->m_name.c_str());
			}
			else {
				a_params->result->SetString(g_selectedNode->m_name.c_str());
			}

			g_selectedMenu = "SEARCHNODE";
		}
	};

	class GetParentNodeNameHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			std::string nodeName = a_params->args[0].GetString();
			if (nodeName.empty()) {
				a_params->result->SetNull();
				return;
			}

			NiNode* node = Nodes::GetNode(g_selectedActor, nodeName);
			if (!node) {
				a_params->result->SetNull();
				return;
			}

			if (!node->m_parent) {
				a_params->result->SetNull();
				return;
			}

			a_params->result->SetString(node->m_parent->m_name.c_str());
		}
	};

	class GetChildNodesHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			a_params->movie->movieRoot->CreateArray(a_params->result);
			std::string nodeName = a_params->args[0].GetString();
			if (nodeName.empty())
				return;

			NiNode* node = Nodes::GetNode(g_selectedActor, nodeName);
			if (!node)
				return;

			for (std::uint32_t ii = 0; ii < node->m_children.m_size; ii++) {
				NiAVObject* childObject = node->m_children.m_data[ii];
				if (!childObject)
					continue;

				NiNode* childNode = childObject->GetAsNiNode();
				if (!childNode)
					continue;

				GFxValue childNodeName;
				childNodeName.SetString(childNode->m_name.c_str());

				a_params->result->PushBack(&childNodeName);
			}
		}
	};

	class GetSavesHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			a_params->movie->movieRoot->CreateArray(a_params->result);

			std::vector<std::string> nodeDataList = ConfigReader::GetNodeDataList();

			for (std::string nodeDataName : nodeDataList) {
				GFxValue fileName;
				fileName.SetString(nodeDataName.c_str());

				a_params->result->PushBack(&fileName);
			}
		}
	};

	class SaveHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			std::string saveName = a_params->args[0].GetString();
			if (saveName.empty()) {
				a_params->result->SetString("ESAVENAME");
				return;
			}

			auto modMapIt = g_modifiedMap.find(g_selectedActor->formID);
			if (modMapIt == g_modifiedMap.end() || modMapIt->second.empty()) {
				a_params->result->SetString("ENOMOD");
				return;
			}

			if (!ConfigReader::SaveNodeData(saveName, g_selectedActor, modMapIt->second)) {
				a_params->result->SetString("ENOPATH");
				return;
			}

			a_params->result->SetString("");
		}
	};

	class LoadSaveHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			std::string saveName = a_params->args[0].GetString();
			if (saveName.empty()) {
				a_params->result->SetString("ESAVENAME");
				return;
			}

			std::unordered_map<std::string, Nodes::NodeData> nodeDataMap = ConfigReader::LoadNodeData(saveName);
			if (nodeDataMap.empty()) {
				a_params->result->SetString("ENOPATH");
				return;
			}

			Nodes::SetNodes(g_selectedActor, nodeDataMap);
			g_selectedMenu = std::string();

			a_params->result->SetString("");
		}
	};

	class DeleteSaveHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* a_params) override {
			std::string saveName = a_params->args[0].GetString();
			if (saveName.empty()) {
				a_params->result->SetString("ESAVENAME");
				return;
			}

			if (!ConfigReader::DeleteNodeData(saveName)) {
				a_params->result->SetString("ENOPATH");
				return;
			}

			a_params->result->SetString("");
		}
	};

	void RegisterMenu() {
		BSFixedString menuName = PNIOPAMenuName.c_str();

		if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName)) {
			(*g_ui)->Register(menuName.c_str(), []() -> IMenu* {
				PNIOPAMenu* menu = PNIOPAMenu::GetSingleton();
				if (!menu)
					menu = new PNIOPAMenu();
				return menu;
				});
		}

		_MESSAGE("Menu Registered");
	}

	void LoadLocalizations() {
		Localizations* loc = Localizations::GetSingleton();

		Setting* setting = GetINISetting("sLanguage:General");
		loc->lang = (setting && setting->GetType() == Setting::kType_String) ? setting->data.s : "en";

		std::string transPath = "Data\\Interface\\Translations\\" + PNIOPAMenuName + "_" + loc->lang + ".txt";
		ConfigReader::FileReader reader(transPath);
		if (!reader.Open()) {
			_MESSAGE("Cannot find the translation file: %s", transPath.c_str());
			return;
		}

		std::string name, value;
		while (reader.GetLine()) {
			if (reader.EmptyOrComment())
				continue;

			name = reader.GetNextData('\t');
			if (name.empty()) {
				_MESSAGE("Cannot read the name: %s", reader.GetCurrentLine().c_str());
				continue;
			}

			value = reader.GetNextData(0);
			if (value.empty()) {
				_MESSAGE("Cannot read the value: %s", reader.GetCurrentLine().c_str());
				continue;
			}

			loc->translationsMap.insert(std::make_pair(name, value));
		}
	}

	void RegisterScaleform(GFxMovieView* a_view, GFxValue* a_f4se_root) {
		RegisterFunction<ScaleformManager::ThrowHandler>(a_f4se_root, a_view->movieRoot, "Throw");
		RegisterFunction<ScaleformManager::GetInitializationDataHandler>(a_f4se_root, a_view->movieRoot, "GetInitializationData");
		RegisterFunction<ScaleformManager::InitializationCompleteHandler>(a_f4se_root, a_view->movieRoot, "InitializationComplete");
		RegisterFunction<ScaleformManager::CloseHandler>(a_f4se_root, a_view->movieRoot, "Close");
		RegisterFunction<ScaleformManager::AllowTextInputHandler>(a_f4se_root, a_view->movieRoot, "AllowTextInput");
		RegisterFunction<ScaleformManager::ToggleFreezeHandler>(a_f4se_root, a_view->movieRoot, "ToggleFreeze");
		RegisterFunction<ScaleformManager::ResetActorHandler>(a_f4se_root, a_view->movieRoot, "ResetActor");
		RegisterFunction<ScaleformManager::SelectNodeHandler>(a_f4se_root, a_view->movieRoot, "SelectNode");
		RegisterFunction<ScaleformManager::GetNodeListHandler>(a_f4se_root, a_view->movieRoot, "GetNodeList");
		RegisterFunction<ScaleformManager::GetNodeNameForSearchHandler>(a_f4se_root, a_view->movieRoot, "GetNodeNameForSearch");
		RegisterFunction<ScaleformManager::GetParentNodeNameHandler>(a_f4se_root, a_view->movieRoot, "GetParentNodeName");
		RegisterFunction<ScaleformManager::GetChildNodesHandler>(a_f4se_root, a_view->movieRoot, "GetChildNodes");
		RegisterFunction<ScaleformManager::GetSavesHandler>(a_f4se_root, a_view->movieRoot, "GetSaves");
		RegisterFunction<ScaleformManager::SaveHandler>(a_f4se_root, a_view->movieRoot, "Save");
		RegisterFunction<ScaleformManager::LoadSaveHandler>(a_f4se_root, a_view->movieRoot, "LoadSave");
		RegisterFunction<ScaleformManager::DeleteSaveHandler>(a_f4se_root, a_view->movieRoot, "DeleteSave");
	}

	void OpenMenu() {
		BSFixedString menuName = PNIOPAMenuName.c_str();

		if (*g_uiMessageManager)
			CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(menuName, kMessage_Open);
	}

	void CloseMenu() {
		BSFixedString menuName(PNIOPAMenuName.c_str());

		if (*g_uiMessageManager)
			CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(menuName, kMessage_Close);
	}

	void ClearMenuSelection() {
		g_selectedMenu = std::string();
		g_selectedGroup = std::string();
	}
}