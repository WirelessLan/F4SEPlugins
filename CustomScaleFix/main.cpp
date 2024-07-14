#include <ShlObj.h>

#include <f4se/PluginAPI.h>
#include <f4se_common/BranchTrampoline.h>

#include "Global.h"
#include "Configs.h"
#include "ZoomData.h"
#include "PlayerAnimEvents.h"
#include "FurniturePositions.h"

PluginHandle			g_pluginHandle;
F4SEMessagingInterface*	g_messaging;

bool bUseCameraOverrideScaleFix = true;
bool bUseCustomFurniturePosition = true;

void OnF4SEMessage(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_NewGame:
	case F4SEMessagingInterface::kMessage_PreLoadGame:
		if (FurniturePositions::ShouldLoadFurnitureConfig())
			FurniturePositions::LoadFurnitureConfig();
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

		if (f4se->runtimeVersion != RUNTIME_VERSION_1_10_984) {
			_MESSAGE("[Critical] unsupported runtime version %d", f4se->runtimeVersion);
			return false;
		}

		g_pluginHandle = f4se->GetPluginHandle();

		// Get the messaging interface
		g_messaging = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging) {
			_MESSAGE("[Critical] couldn't get messaging interface");
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se) {
		_MESSAGE("[Info] %s v%d.%d.%d Loaded", PLUGIN_NAME, (PLUGIN_VERSION >> 24) & 0xFF, (PLUGIN_VERSION >> 16) & 0xFF, (PLUGIN_VERSION >> 4) & 0xFF);

		if (!g_branchTrampoline.Create(static_cast<size_t>(1024 * 64))) {
			_ERROR("[Critical] couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return false;
		}
		if (!g_localTrampoline.Create(static_cast<size_t>(1024 * 64), nullptr)) {
			_ERROR("[Critical] couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return false;
		}

		Configs::GetConfigValue("Options", "bUseCameraOverrideScaleFix", &bUseCameraOverrideScaleFix);
		Configs::GetConfigValue("Options", "bUseCustomFurniturePosition", &bUseCustomFurniturePosition);
		
		_MESSAGE("[Info] bUseCameraOverrideScaleFix: %d", bUseCameraOverrideScaleFix);
		_MESSAGE("[Info] bUseCustomFurniturePosition: %d", bUseCustomFurniturePosition);

		ZoomData::Hooks_GetGeometryOffset();

		if (bUseCameraOverrideScaleFix)
			PlayerAnimEvents::Hooks_PlayerAnimEvents();

		/*if (bUseCustomFurniturePosition) {
			FurniturePositions::Hooks_PlayFurnitureAnimation();
			FurniturePositions::Hooks_InitializeChairBedQuickPosition();

			if (g_messaging)
				g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);
		}*/

		return true;
	}

	F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,

		PLUGIN_VERSION,
		PLUGIN_NAME,
		"VG",

		0,	// not version independent
		0,	// not version independent (extended field)
		{ RUNTIME_VERSION_1_10_984, 0 },	// compatible with 1.10.980

		0,	// works with any version of the script extender. you probably do not need to put anything here
	};
};