// F4SE
#include "common/IDebugLog.h"  // IDebugLog

#include <shlobj.h>	// CSIDL_MYCODUMENTS

// User Defined Header
#include "Global.h"

F4SEPapyrusInterface* g_papyrus;

VMArray<TESForm*> GetCoordiFromFile(StaticFunctionTag*, BSFixedString slot) {
	return SlotManager::ReadSlot(slot);
}

bool SaveCoordiToFile(StaticFunctionTag*, BSFixedString slot) {
	return SlotManager::SaveSlot(slot);
}

bool RegisterFuncs(VirtualMachine* vm) {
	vm->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, VMArray<TESForm*>, BSFixedString>("GetCoordiFromFile", "CoordiSaver", GetCoordiFromFile, vm));
	vm->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, bool, BSFixedString>("SaveCoordiToFile", "CoordiSaver", SaveCoordiToFile, vm));
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

		g_papyrus = (F4SEPapyrusInterface*)f4se->QueryInterface(kInterface_Papyrus);
		if (!g_papyrus)	{
			_FATALERROR("couldn't get papyrus interface");
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se) {
		_MESSAGE("%s Loaded", PLUGIN_NAME);

		if (g_papyrus)
			g_papyrus->Register(RegisterFuncs);

		return true;
	}
};