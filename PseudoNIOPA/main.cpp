// F4SE
#include "common/IDebugLog.h"  // IDebugLog

#include <shlobj.h>	// CSIDL_MYCODUMENTS

// User Defined Header
#include "Global.h"

PluginHandle			g_pluginHandle;
F4SEMessagingInterface*	g_messaging;
F4SEPapyrusInterface*	g_papyrus;
F4SEScaleformInterface* g_scaleform;

PluginSettings			g_pluginSettings;

void OnF4SEMessage(F4SEMessagingInterface::Message* a_msg) {
	switch (a_msg->type) {
	case F4SEMessagingInterface::kMessage_GameLoaded:
		ScaleformManager::RegisterMenu();
		ScaleformManager::LoadLocalizations();
		Inputs::RegisterInputHandler();
		Events::Attach_Events();
		break;

	case F4SEMessagingInterface::kMessage_NewGame:
	case F4SEMessagingInterface::kMessage_PostLoadGame:
		ScaleformManager::ClearMenuSelection();
		InputEnableManager::ResetInputEnableLayer();
		Utility::UnfreezeActor(*g_player);
		Nodes::ClearAll();
		Inputs::ClearInputs();
		break;
	}
}

bool RegisterFuncs(VirtualMachine* a_vm) {
	PapyrusFuncs::Register(a_vm);
	return true;
}

bool RegisterScaleform(GFxMovieView* a_view, GFxValue* a_f4se_root) {
	ScaleformManager::RegisterScaleform(a_view, a_f4se_root);
	return true;
}

std::uint32_t ReadKey(const std::string& a_str) {
	char* p;
	std::uint32_t value = strtoul(a_str.c_str(), &p, 16);
	if (*p == 0)
		return value;
	return 0;
}

void ReadINIFile() {
	std::uint32_t value;
	std::string data;

	if (ConfigReader::GetConfigValue("Keys", "DirXKey", &data)) {
		value = ReadKey(data);
		if (value)
			g_pluginSettings.DirXKey = value;
	}
	_MESSAGE("DirXKey: 0x%02X", g_pluginSettings.DirXKey);

	if (ConfigReader::GetConfigValue("Keys", "DirYKey", &data)) {
		value = ReadKey(data);
		if (value)
			g_pluginSettings.DirYKey = value;
	}
	_MESSAGE("DirYKey: 0x%02X", g_pluginSettings.DirYKey);

	if (ConfigReader::GetConfigValue("Keys", "DirZKey", &data)) {
		value = ReadKey(data);
		if (value)
			g_pluginSettings.DirZKey = value;
	}
	_MESSAGE("DirZKey: 0x%02X", g_pluginSettings.DirZKey);

	if (ConfigReader::GetConfigValue("Keys", "IncTransKey", &data)) {
		value = ReadKey(data);
		if (value)
			g_pluginSettings.IncTransKey = value;
	}
	_MESSAGE("IncTransKey: 0x%02X", g_pluginSettings.IncTransKey);

	if (ConfigReader::GetConfigValue("Keys", "DecTransKey", &data)) {
		value = ReadKey(data);
		if (value)
			g_pluginSettings.DecTransKey = value;
	}
	_MESSAGE("DecTransKey: 0x%02X", g_pluginSettings.DecTransKey);

	if (ConfigReader::GetConfigValue("Keys", "IncRotKey", &data)) {
		value = ReadKey(data);
		if (value)
			g_pluginSettings.IncRotKey = value;
	}
	_MESSAGE("IncRotKey: 0x%02X", g_pluginSettings.IncRotKey);

	if (ConfigReader::GetConfigValue("Keys", "DecRotKey", &data)) {
		value = ReadKey(data);
		if (value)
			g_pluginSettings.DecRotKey = value;
	}
	_MESSAGE("DecRotKey: 0x%02X", g_pluginSettings.DecRotKey);

	if (ConfigReader::GetConfigValue("Keys", "IncScaleKey", &data)) {
		value = ReadKey(data);
		if (value)
			g_pluginSettings.IncScaleKey = value;
	}
	_MESSAGE("IncScaleKey: 0x%02X", g_pluginSettings.IncScaleKey);

	if (ConfigReader::GetConfigValue("Keys", "DecScaleKey", &data)) {
		value = ReadKey(data);
		if (value)
			g_pluginSettings.DecScaleKey = value;
	}
	_MESSAGE("DecScaleKey: 0x%02X", g_pluginSettings.DecScaleKey);
}

/* Plugin Query */
extern "C" {
	bool F4SEPlugin_Query(const F4SEInterface* a_f4se, PluginInfo* a_info) {
		std::string logPath{ "\\My Games\\Fallout4\\F4SE\\" PLUGIN_NAME ".log" };
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath.c_str());
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = PLUGIN_NAME;
		a_info->version = PLUGIN_VERSION;

		if (a_f4se->runtimeVersion != RUNTIME_VERSION_1_10_163) {
			_MESSAGE("unsupported runtime version %d", a_f4se->runtimeVersion);
			return false;
		}

		g_pluginHandle = a_f4se->GetPluginHandle();

		// Get the messaging interface
		g_messaging = (F4SEMessagingInterface*)a_f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging) {
			_MESSAGE("couldn't get messaging interface");
			return false;
		}

		g_papyrus = (F4SEPapyrusInterface*)a_f4se->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus) {
			_FATALERROR("couldn't get papyrus interface");
			return false;
		}

		g_scaleform = (F4SEScaleformInterface*)a_f4se->QueryInterface(kInterface_Scaleform);
		if (!g_scaleform) {
			_FATALERROR("couldn't get scaleform interface");
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* a_f4se) {
		_MESSAGE("%s v%d.%d.%d Loaded", PLUGIN_NAME, (PLUGIN_VERSION >> 24) & 0xFF, (PLUGIN_VERSION >> 16) & 0xFF, (PLUGIN_VERSION >> 4) & 0xFF);

		ReadINIFile();

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);

		if (g_papyrus)
			g_papyrus->Register(RegisterFuncs);

		if (g_scaleform)
			g_scaleform->Register(PLUGIN_NAME, RegisterScaleform);

		return true;
	}
};