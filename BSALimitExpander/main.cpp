#include <ShlObj.h>

#include <f4se/PluginAPI.h>
#include <f4se_common/BranchTrampoline.h>

#include "Version.h"
#include "BSResource.h"
#include "BSTextureIndex.h"

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

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* a_f4se) {
		_MESSAGE("[Info] %s v%d.%d.%d Loaded", PLUGIN_NAME, (PLUGIN_VERSION >> 24) & 0xFF, (PLUGIN_VERSION >> 16) & 0xFF, (PLUGIN_VERSION >> 4) & 0xFF);

		if (!g_branchTrampoline.Create(static_cast<std::size_t>(256))) {
			_ERROR("[Critical] Couldn't create branch trampoline. Stopping plugin load.");
			return false;
		}

		if (!g_localTrampoline.Create(static_cast<std::size_t>(256), nullptr)) {
			_ERROR("[Critical] Couldn't create codegen buffer. Stopping plugin load.");
			return false;
		}

		BSResource::Archive2::Hooks_AddDataFile();
		BSResource::SDirectory2::Hooks_Retrieve();
		BSTextureIndex::Overwrites_Instruction();
		BSTextureIndex::Hooks_AddDataFile();

		_MESSAGE("[Info] Hooking successfully completed!");

		return true;
	}
};