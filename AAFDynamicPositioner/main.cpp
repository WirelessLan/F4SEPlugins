// F4SE
#include "common/IDebugLog.h"  // IDebugLog

#include <shlobj.h>	// CSIDL_MYCODUMENTS

// User Defined Header
#include "Global.h"

PluginHandle			g_pluginHandle;
F4SEMessagingInterface* g_messaging;
F4SEPapyrusInterface*	g_papyrus;
F4SEScaleformInterface* g_scaleform;

PluginSettings			g_pluginSettings;

void OnF4SEMessage(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_GameLoaded:
		Scaleform::PositionerMenu::RegisterMenu();
		break;

	case F4SEMessagingInterface::kMessage_NewGame:
	case F4SEMessagingInterface::kMessage_PreLoadGame:
		Positioner::ResetPositioner();
		break;
	}
}

bool RegisterPapyrusFunctions(VirtualMachine* vm) {
	Positioner::RegisterPositionerFunctions(vm);
	return true;
}

bool RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root) {
	Scaleform::RegisterScaleform(view, f4se_root);
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

		g_papyrus = (F4SEPapyrusInterface*)f4se->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus) {
			_FATALERROR("couldn't get papyrus interface");
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

		ConfigReader::GetConfigValue("Settings", "bSeparatePlayerOffset", &g_pluginSettings.bSeparatePlayerOffset);
		ConfigReader::GetConfigValue("Settings", "bUnifyAAFDoppelgangerScale", &g_pluginSettings.bUnifyAAFDoppelgangerScale);
		ConfigReader::GetConfigValue("Settings", "fMoveAxisSize", &g_pluginSettings.fMoveAxisSize);
		ConfigReader::GetConfigValue("Settings", "iPlayerPositionerType", (SInt32*)&g_pluginSettings.iPlayerPositionerType);
		ConfigReader::GetConfigValue("Settings", "iNPCPositionerType", (SInt32*)&g_pluginSettings.iNPCPositionerType);
		
		_MESSAGE("bSeparatePlayerOffset: %d", g_pluginSettings.bSeparatePlayerOffset);
		_MESSAGE("bUnifyAAFDoppelgangerScale: %d", g_pluginSettings.bUnifyAAFDoppelgangerScale);
		_MESSAGE("fMoveAxisSize: %f", g_pluginSettings.fMoveAxisSize);
		_MESSAGE("iPlayerPositionerType: %d", g_pluginSettings.iPlayerPositionerType);
		_MESSAGE("iNPCPositionerType: %d", g_pluginSettings.iNPCPositionerType);

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);

		if (g_papyrus)
			g_papyrus->Register(RegisterPapyrusFunctions);

		if (g_scaleform)
			g_scaleform->Register("AAFDynamicPositioner", RegisterScaleform);

		return true;
	}
};