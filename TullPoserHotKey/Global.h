#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameData.h>
#include <f4se/GameReferences.h>
#include <f4se/GameMenus.h>
#include <f4se/GameRTTI.h>
#include <f4se/ScaleformLoader.h>
#include <f4se/PapyrusArgs.h>
#include <f4se/PapyrusNativeFunctions.h>

#include <f4se_common/f4se_version.h>
#include <f4se_common/Relocation.h>

#include <fstream>
#include <vector>
#include <map>

#include "ScaleformManager.h"
#include "PapyrusManager.h"
#include "ConfigReader.h"
#include "Utility.h"

#define PLUGIN_NAME	"TullPoserHotKey"
#define PLUGIN_VERSION MAKE_EXE_VERSION(0, 1, 2)

struct CaseInsensitiveComparator {
	bool operator()(const std::string& a, const std::string& b) const noexcept {
		return _stricmp(a.c_str(), b.c_str()) < 0;
	}
};

template <typename T>
using CaseInsensitiveMap = std::map<std::string, T, CaseInsensitiveComparator>;

extern std::vector<std::string> g_pluginVec;
extern CaseInsensitiveMap<CaseInsensitiveMap<TESIdleForm*>> g_idleAnimMap;
extern CaseInsensitiveMap<std::vector<std::string>> g_customPoseMap;

extern std::string g_selectedPlugin;
extern std::string g_selectedPose;