#include <ShlObj.h>

#include <f4se/PluginAPI.h>

#include "Configs.h"
#include "Hooks.h"
#include "Version.h"

PluginHandle			g_pluginHandle;
F4SEMessagingInterface* g_messaging;

void F4SEMessageHandler(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_GameLoaded:
		Hooks::Install();
		break;
	}
}

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
			_MESSAGE("[Error] unsupported runtime version %d", f4se->runtimeVersion);
			return false;
		}

		g_pluginHandle = f4se->GetPluginHandle();

		g_messaging = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging) {
			_FATALERROR("[Fatal] couldn't get messaging interface");
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se) {
		_MESSAGE("[Info] %s v%d.%d.%d Loaded", PLUGIN_NAME, (PLUGIN_VERSION >> 24) & 0xFF, (PLUGIN_VERSION >> 16) & 0xFF, (PLUGIN_VERSION >> 4) & 0xFF);

		Configs::LoadConfig();

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", F4SEMessageHandler);

		return true;
	}
};