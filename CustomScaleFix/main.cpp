// F4SE
#include "common/IDebugLog.h"  // IDebugLog

#include <shlobj.h>	// CSIDL_MYCODUMENTS

#include "Global.h"

PluginHandle			g_pluginHandle;
F4SEMessagingInterface*	g_messaging;

PluginOptions			g_pluginOptions;

void OnF4SEMessage(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_GameLoaded:
		if (g_pluginOptions.useUnifyAAFDoppelgangerScale)
			GetEventDispatcher<TESSceneActionEvent>()->AddEventSink(new SceneActionEventReceiver());
		break;
	case F4SEMessagingInterface::kMessage_NewGame:
	case F4SEMessagingInterface::kMessage_PreLoadGame:
		if (g_pluginOptions.useCustomFurniturePosition && ShouldLoadFurnitureConfig()) {
			_MESSAGE("Load Furniture Config...");
			LoadFurnitureConfig();
		}
		break;
	}
}

/* Plugin Query */
extern "C" {
	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info) {
		std::string logPath{ "\\My Games\\Fallout4\\F4SE\\" PLUGIN_NAME ".log" };
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath.c_str());
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		// populate info structure
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
			_MESSAGE("couldn't get messaging interface");
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
		if (!g_localTrampoline.Create(1024 * 64, nullptr)) {
			_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return false;
		}

		GetConfigValue("Options", "bUseCameraOverrideScaleFix", &g_pluginOptions.useCameraOverrideScaleFix);
		GetConfigValue("Options", "bUseUnifyAAFDoppelgangerScale", &g_pluginOptions.useUnifyAAFDoppelgangerScale);
		GetConfigValue("Options", "bUseCustomFurniturePosition", &g_pluginOptions.useCustomFurniturePosition);
		GetConfigValue("Options", "bPlayerOnlyCustomFurniturePosition", &g_pluginOptions.playerOnlyCustomFurniturePosition);
		GetConfigValue("Options", "uFurniturePositionType", &g_pluginOptions.furniturePositionType);
		
		_MESSAGE("bUseCameraOverrideScaleFix: %d", g_pluginOptions.useCameraOverrideScaleFix);
		_MESSAGE("bUseUnifyAAFDoppelgangerScale: %d", g_pluginOptions.useUnifyAAFDoppelgangerScale);
		_MESSAGE("bUseCustomFurniturePosition: %d", g_pluginOptions.useCustomFurniturePosition);
		_MESSAGE("bPlayerOnlyCustomFurniturePosition: %d", g_pluginOptions.playerOnlyCustomFurniturePosition);
		_MESSAGE("uFurniturePositionType: %u", g_pluginOptions.furniturePositionType);

		Hooks_GetZoomData();

		if (g_pluginOptions.useCameraOverrideScaleFix)
			Hooks_PlayerAnimationEvent();
		if (g_pluginOptions.useCustomFurniturePosition)
			Hooks_GetFurniturePosition();

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);

		return true;
	}
};