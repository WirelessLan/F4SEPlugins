#include "Global.h"

namespace ScaleformManager {
	const std::string CoordiSaverMenuName = "CoordiSaverMenu";
	bool g_menuAllowTextInput = false;
	CoordiSaverMenu* g_csMenu;

	MenuInputHandler g_csMenuInputHandler;

	void RegisterMenuInputHandler(bool bReg) {
		if (bReg) {
			g_csMenuInputHandler.enabled = true;
			tArray<BSInputEventUser*>* inputEvents = &((*g_menuControls)->inputEvents);
			BSInputEventUser* inputHandler = &g_csMenuInputHandler;
			int idx = inputEvents->GetItemIndex(inputHandler);
			if (idx == -1) {
				inputEvents->Push(&g_csMenuInputHandler);
				_MESSAGE("Menu Input Handler Registered");
			}
		}
		else {
			g_csMenuInputHandler.enabled = false;
		}
	}

	void MenuInputHandler::OnButtonEvent(ButtonEvent* inputEvent) {
		if (!g_csMenu)
			return;

		if (inputEvent->deviceType != InputEvent::kDeviceType_Keyboard)
			return;

		UInt32 keyCode = inputEvent->keyMask;
		bool  isDown = inputEvent->isDown == 1.0f && inputEvent->timer == 0.0f;

		if (keyCode == 0x09 && isDown) {
			if (g_menuAllowTextInput)
				return;

			GFxMovieRoot* movieRoot = g_csMenu->movie->movieRoot;
			GFxValue root;

			if (!movieRoot->GetVariable(&root, "root")) {
				_ERROR("Couldn't get a root");
				return;
			}

			GFxValue closeAll;
			movieRoot->CreateObject(&closeAll);
			closeAll.SetBool(false);

			root.Invoke("CloseMenu", nullptr, &closeAll, 1);
		}
	}

	CoordiSaverMenu::CoordiSaverMenu() {
		flags = kFlag_UsesCursor | kFlag_PausesGame;
		CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(this, movie, "CoordiSaverMenu", "root1", 4);
	}

	IMenu* CoordiSaverMenu::CreateMenu() {
		return (g_csMenu = new CoordiSaverMenu());
	}

	void CoordiSaverMenu::RegisterMenu() {
		static BSFixedString menuName(CoordiSaverMenuName.c_str());
		if ((*g_ui) != nullptr && !(*g_ui)->IsMenuRegistered(menuName))
			(*g_ui)->Register(menuName.c_str(), CreateMenu);

		_MESSAGE("Menu Registered");
	}

	void CoordiSaverMenu::OpenMenu() {
		static BSFixedString menuName(CoordiSaverMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Open);
	}

	void CoordiSaverMenu::CloseMenu() {
		static BSFixedString menuName(CoordiSaverMenuName.c_str());
		CALL_MEMBER_FN((*g_uiMessageManager), SendUIMessage)(menuName, kMessage_Close);
	}

	void MenuInitializeHandler::Invoke(Args* args) {
		RegisterMenuInputHandler(true);

		GFxMovieRoot* movieRoot = args->movie->movieRoot;
		GFxValue root;

		if (!movieRoot->GetVariable(&root, "root")) {
			_ERROR("Couldn't get a root");
			return;
		}

		root.Invoke("ShowMenu", nullptr, nullptr, 0);
	}

	void GetSlotListHandler::Invoke(Args* args) {
		args->movie->movieRoot->CreateArray(args->result);

		std::string directory = "Data\\F4SE\\Plugins\\" + std::string(PLUGIN_NAME) + "_Slots\\";
		const char* match = "*.codss";

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

	void LoadSlotHandler::Invoke(Args* args) {
		if (args->numArgs == 1) {
			std::string slotName = args->args[0].GetString();
			Util::LoadSlot(slotName);
		}
	}

	void AddSlotHandler::Invoke(Args* args)	{
		if (args->numArgs == 1) {
			std::string slotName = args->args[0].GetString();
			args->result->SetBool(SlotManager::SaveSlot(slotName));
		}
	}

	void DeleteSlotHandler::Invoke(Args* args) {
		if (args->numArgs == 1) {
			std::string slotName = args->args[0].GetString();
			args->result->SetBool(SlotManager::DeleteSlot(slotName));
		}
	}

	void MenuCloseHandler::Invoke(Args* args) {
		RegisterMenuInputHandler(false);
		CoordiSaverMenu::CloseMenu();
	}

	void AllowTextInputHandler::Invoke(Args* args) {
		bool allowInput = args->args[0].GetBool();
		g_menuAllowTextInput = allowInput;
		(*g_inputMgr)->AllowTextInput(allowInput);
	}
}