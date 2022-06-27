// F4SE
#include "common/IDebugLog.h"  // IDebugLog

#include <shlobj.h>	// CSIDL_MYCODUMENTS

// User Defined Header
#include "Global.h"

PluginHandle			g_pluginHandle;
F4SEMessagingInterface*	g_messaging;
F4SEPapyrusInterface*	g_papyrus;

PluginSettings			g_pluginSettings;

void OnF4SEMessage(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_GameLoaded:
		Inputs::RegisterInputHandler();
		break;

	case F4SEMessagingInterface::kMessage_NewGame:
	case F4SEMessagingInterface::kMessage_PreLoadGame:
	case F4SEMessagingInterface::kMessage_PreSaveGame:
		Nodes::ClearAll();
		break;
	}
}

bool RegisterFuncs(VirtualMachine* vm) {
	PapyrusFuncs::Register(vm);
	return true;
}

UInt32 ReadKey(const std::string& str) {
	char* p;
	UInt32 value;
	value = strtoul(str.c_str(), &p, 16);
	if (*p == 0)
		return value;
	return 0;
}

void ReadINIFile() {
	UInt32 value;
	std::string data;

	if (ConfigReader::GetConfigValue("Keys", "ShowMenuKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.ShowMenuKey = value;
			_MESSAGE("ShowMenuKey: [0x%02X]", g_pluginSettings.ShowMenuKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "ClearSelectKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.ClearSelectKey = value;
			_MESSAGE("ClearSelectKey: [0x%02X]", g_pluginSettings.ClearSelectKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "ResetSelectedNodeKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.ResetSelectedNodeKey = value;
			_MESSAGE("ResetSelectedNodeKey: [0x%02X]", g_pluginSettings.ResetSelectedNodeKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "DirXKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.DirXKey = value;
			_MESSAGE("DirXKey: [0x%02X]", g_pluginSettings.DirXKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "DirYKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.DirYKey = value;
			_MESSAGE("DirYKey: [0x%02X]", g_pluginSettings.DirYKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "DirZKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.DirZKey = value;
			_MESSAGE("DirZKey: [0x%02X]", g_pluginSettings.DirZKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "IncTransKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.IncTransKey = value;
			_MESSAGE("IncTransKey: [0x%02X]", g_pluginSettings.IncTransKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "DecTransKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.DecTransKey = value;
			_MESSAGE("DecTransKey: [0x%02X]", g_pluginSettings.DecTransKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "IncRotKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.IncRotKey = value;
			_MESSAGE("IncRotKey: [0x%02X]", g_pluginSettings.IncRotKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "DecRotKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.DecRotKey = value;
			_MESSAGE("DecRotKey: [0x%02X]", g_pluginSettings.DecRotKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "IncScaleKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.IncScaleKey = value;
			_MESSAGE("IncScaleKey: [0x%02X]", g_pluginSettings.IncScaleKey);
		}
	}
	if (ConfigReader::GetConfigValue("Keys", "DecScaleKey", &data)) {
		value = ReadKey(data);
		if (value) {
			g_pluginSettings.DecScaleKey = value;
			_MESSAGE("DecScaleKey: [0x%02X]", g_pluginSettings.DecScaleKey);
		}
	}
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
			_MESSAGE("couldn't get messaging interface");
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

		ReadINIFile();

		Hooks::Install_PipboyHandlerHook();

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);

		if (g_papyrus)
			g_papyrus->Register(RegisterFuncs);

		return true;
	}
};