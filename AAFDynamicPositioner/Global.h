#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameReferences.h>
#include <f4se/GameData.h>
#include <f4se/GameExtraData.h>
#include <f4se/GameMenus.h>
#include <f4se/GameRTTI.h>
#include <f4se/NiNodes.h>
#include <f4se/PapyrusVM.h>
#include <f4se/PapyrusNativeFunctions.h>
#include <f4se/PapyrusEvents.h>
#include <f4se/ScaleformCallbacks.h>
#include <f4se/ScaleformValue.h>
#include <f4se/ScaleformLoader.h>
#include <f4se/InputMap.h>

#include <f4se_common/f4se_version.h>

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>

#include "Utility.h"
#include "ConfigReader.h"
#include "Positioner.h"
#include "PositionData.h"
#include "Scaleform.h"
#include "InputEnable.h"

#define PLUGIN_NAME	"AAFDynamicPositioner"
#define PLUGIN_VERSION MAKE_EXE_VERSION(0, 5, 2)

enum PositionerType {
	kPositionerType_Relative = 0,
	kPositionerType_Absolute = 1
};

struct PluginSettings {
	bool bSeparatePlayerOffset = false;
	bool bUnifyAAFDoppelgangerScale = true;
	float fMoveAxisSize = 1.0f;
	PositionerType iPlayerPositionerType = PositionerType::kPositionerType_Relative;
	PositionerType iNPCPositionerType = PositionerType::kPositionerType_Relative;
};

extern PluginSettings g_pluginSettings;

extern std::unordered_map<UInt32, ActorData> actorMap;
extern std::unordered_map<UInt64, SceneData> sceneMap;