#include <ShlObj.h>

#include <f4se/PluginAPI.h>
#include <f4se_common/BranchTrampoline.h>

#include "Global.h"
#include "CACS.h"
#include "Hooks.h"

PluginHandle			g_pluginHandle = kPluginHandle_Invalid;
F4SEMessagingInterface* g_messaging = NULL;

bool bDebug = false;

void ReadConfig() {
	const std::string configPath{ "Data\\F4SE\\Plugins\\" PLUGIN_NAME ".ini" };
	char result[256] = { 0 };
	GetPrivateProfileString("Debug", "bDebug", NULL, result, sizeof(result), configPath.c_str());

	std::string sResult = result;
	if (!sResult.empty())
		bDebug = std::stoul(sResult, nullptr, 10);

	_MESSAGE("bDebug: %d", bDebug);
}

void OnF4SEMessage(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_NewGame:
	case F4SEMessagingInterface::kMessage_PreLoadGame:
		Hooks::SetModelProcessor();
		Hooks::ClearPathMap();

		if (CACS::ShouldLoadRules()) {
			_MESSAGE("Loading Rules...");
			CACS::LoadRules();
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
		_MESSAGE("%s v%d.%d.%d Loaded", PLUGIN_NAME, (PLUGIN_VERSION >> 24) & 0xFF, (PLUGIN_VERSION >> 16) & 0xFF, (PLUGIN_VERSION >> 4) & 0xFF);

		if (!g_branchTrampoline.Create(static_cast<size_t>(1024 * 64))) {
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return false;
		}
		if (!g_localTrampoline.Create(static_cast<size_t>(1024 * 64), nullptr))
		{
			_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return false;
		}

		ReadConfig();

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);

		Hooks::Hooks_ReplaceRefModel();
		Hooks::Hooks_PrepareName();

		return true;
	}
};