// F4SE
#include "common/IDebugLog.h"  // IDebugLog

#include <shlobj.h>	// CSIDL_MYCODUMENTS

// User Defined Header
#include "Global.h"

PluginHandle			g_pluginHandle = kPluginHandle_Invalid;
F4SEMessagingInterface* g_messaging = nullptr;
F4SEScaleformInterface* g_scaleform = nullptr;
F4SEPapyrusInterface*	g_papyrus = nullptr;

std::vector<std::string> g_pluginVec;
CaseInsensitiveMap<CaseInsensitiveMap<TESIdleForm*>> g_idleAnimMap;

void InitializeConfig() {
	if (ConfigReader::ShouldReadConfig())
		g_pluginVec = ConfigReader::ReadConfig();
}

void OnF4SEMessage(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_GameLoaded:
		ScaleformManager::TullPoserHotKeyMenu::RegisterMenu();
		break;

	case F4SEMessagingInterface::kMessage_PreLoadGame:
		InitializeConfig();
		ScaleformManager::ClearSelectedVars();

		break;
	}
}

bool RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root) {
	RegisterFunction<ScaleformManager::InitializeHandler>(f4se_root, view->movieRoot, "Initialize");
	RegisterFunction<ScaleformManager::CloseHandler>(f4se_root, view->movieRoot, "Close");
	RegisterFunction<ScaleformManager::GetPluginListHandler>(f4se_root, view->movieRoot, "GetPluginList");
	RegisterFunction<ScaleformManager::SelectPluginHandler>(f4se_root, view->movieRoot, "SelectPlugin");
	RegisterFunction<ScaleformManager::GetPoseListHandler>(f4se_root, view->movieRoot, "GetPoseList");
	RegisterFunction<ScaleformManager::SelectPoseHandler>(f4se_root, view->movieRoot, "SelectPose");
	return true;
}

bool RegisterFuncs(VirtualMachine* vm) {
	vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("ShowMenu", "TullPoserHotKey", PapyrusManager::ShowMenu, vm));
	vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>("StopIdle", "TullPoserHotKey", PapyrusManager::StopIdle, vm));
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

		g_papyrus = (F4SEPapyrusInterface*)f4se->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus) {
			_FATALERROR("couldn't get papyrus interface");
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se) {
		_MESSAGE("%s Loaded", PLUGIN_NAME);

		if (!g_branchTrampoline.Create(1024 * 64)) {
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return false;
		}
		if (!g_localTrampoline.Create(1024 * 64, nullptr))
		{
			_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return false;
		}

		Hook::Hooks_IdleReady();

		InitializeConfig();

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);

		if (g_scaleform)
			g_scaleform->Register(PLUGIN_NAME, RegisterScaleform);

		if (g_papyrus)
			g_papyrus->Register(RegisterFuncs);

		return true;
	}
};