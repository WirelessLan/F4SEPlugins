// F4SE
#include "common/IDebugLog.h"  // IDebugLog

#include <shlobj.h>	// CSIDL_MYCODUMENTS

// User Defined Header
#include "Global.h"

PluginHandle			g_pluginHandle = kPluginHandle_Invalid;
F4SEMessagingInterface*	g_messaging = nullptr;
F4SEScaleformInterface*	g_scaleform = nullptr;
F4SEPapyrusInterface*	g_papyrus = nullptr;

VMArray<TESForm*> LoadSlotFromFile(StaticFunctionTag*, BSFixedString slot) {
	return SlotManager::LoadSlot(slot);
}

bool SaveSlotToFile(StaticFunctionTag*, BSFixedString slot) {
	return SlotManager::SaveSlot(slot);
}

UInt32 HexToInt(StaticFunctionTag*, BSFixedString hex) {
	return std::stoul(hex.c_str(), nullptr, 16);
}

void ShowMenu(StaticFunctionTag*) {
	ScaleformManager::CoordiSaverMenu::OpenMenu();
}

bool RegisterFuncs(VirtualMachine* vm) {
	vm->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, VMArray<TESForm*>, BSFixedString>("LoadSlotFromFile", "CoordiSaver", LoadSlotFromFile, vm));
	vm->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("SaveSlotToFile", "CoordiSaver", SaveSlotToFile, vm));
	vm->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, void>("ShowMenu", "CoordiSaver", ShowMenu, vm));
	return true;
}

void OnF4SEMessage(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_GameLoaded:
		ScaleformManager::CoordiSaverMenu::RegisterMenu();
		break;
	}
}

bool RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root) {
	RegisterFunction<ScaleformManager::MenuInitializeHandler>(f4se_root, view->movieRoot, "Initialize");
	RegisterFunction<ScaleformManager::GetSlotListHandler>(f4se_root, view->movieRoot, "GetSlotList");
	RegisterFunction<ScaleformManager::LoadSlotHandler>(f4se_root, view->movieRoot, "LoadSlot");
	RegisterFunction<ScaleformManager::AddSlotHandler>(f4se_root, view->movieRoot, "AddSlot");
	RegisterFunction<ScaleformManager::DeleteSlotHandler>(f4se_root, view->movieRoot, "DeleteSlot");
	RegisterFunction<ScaleformManager::MenuCloseHandler>(f4se_root, view->movieRoot, "Close");
	RegisterFunction<ScaleformManager::AllowTextInputHandler>(f4se_root, view->movieRoot, "AllowTextInput");

	return true;
}

/* Plugin Query */
extern "C" {
	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info) {
		std::string logPath{ "\\My Games\\Fallout4\\F4SE\\" PLUGIN_NAME ".log" };
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath.c_str());
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PLUGIN_NAME;
		info->version = PLUGIN_VERSION;

		if (f4se->runtimeVersion != RUNTIME_VERSION_1_10_163) {
			_MESSAGE("unsupported runtime version %d", f4se->runtimeVersion);
			return false;
		}

		g_pluginHandle = f4se->GetPluginHandle();

		g_papyrus = (F4SEPapyrusInterface*)f4se->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus) {
			_FATALERROR("couldn't get papyrus interface");
			return false;
		}

		// Get the messaging interface
		g_messaging = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging) {
			_FATALERROR("couldn't get messaging interface");
			return false;
		}

		g_scaleform = (F4SEScaleformInterface*)f4se->QueryInterface(kInterface_Scaleform);
		if (!g_scaleform) {
			_FATALERROR("couldn't get scaleform interface");
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se) {
		_MESSAGE("%s Loaded", PLUGIN_NAME);

		if (g_papyrus)
			g_papyrus->Register(RegisterFuncs);

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);

		if (g_scaleform)
			g_scaleform->Register(PLUGIN_NAME, RegisterScaleform);

		return true;
	}
};