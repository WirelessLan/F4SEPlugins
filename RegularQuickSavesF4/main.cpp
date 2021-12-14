// F4SE
#include "common/IDebugLog.h"  // IDebugLog
#include <f4se/PluginAPI.h>

// Common
#include "f4se_common/f4se_version.h"
#include "f4se_common/Relocation.h"
#include "f4se_common/SafeWrite.h"

#include <shlobj.h>	// CSIDL_MYCODUMENTS

#define PLUGIN_NAME	"RegularQuickSavesF4"

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
		info->version = MAKE_EXE_VERSION(1, 0, 0);

		if (f4se->runtimeVersion != RUNTIME_VERSION_1_10_163) {
			_MESSAGE("unsupported runtime version %d", f4se->runtimeVersion);
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se) {
		_MESSAGE("%s Loaded", PLUGIN_NAME);

		UInt32 regularSave = 0x00000002;
		UInt32 lastSaveLoad = 0x00000004;

		uintptr_t quickSave_Key = RelocAddr<uintptr_t>(0x12AB6F0 + 0x7E);
		uintptr_t quickSave_Menu = RelocAddr<uintptr_t>(0xB7F6C0 + 0x206);
		uintptr_t quickLoad_Key = RelocAddr<uintptr_t>(0x12AB6F0 + 0xB8);
		SafeWrite32(quickSave_Key, regularSave);
		SafeWrite32(quickSave_Menu, regularSave);
		SafeWrite32(quickLoad_Key, lastSaveLoad);

		return true;
	}
};