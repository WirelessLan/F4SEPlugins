#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameReferences.h>
#include <f4se/GameObjects.h>
#include <f4se/GameExtraData.h>
#include <f4se/GameRTTI.h>
#include <f4se/GameInput.h>
#include <f4se/NiNodes.h>
#include <f4se/PapyrusNativeFunctions.h>

#include <f4se_common/f4se_version.h>
#include <f4se_common/SafeWrite.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "PapyrusFuncs.h"
#include "Nodes.h"
#include "Inputs.h"
#include "Hooks.h"
#include "Utility.h"
#include "ConfigReader.h"

#define PLUGIN_NAME	"PseudoNIOPA"
#define PLUGIN_VERSION MAKE_EXE_VERSION(0, 0, 1)

struct PluginSettings {
	UInt32 DirXKey = 0xBC;
	UInt32 DirYKey = 0xBE;
	UInt32 DirZKey = 0xBF;
	UInt32 IncTransKey = 0x26;
	UInt32 DecTransKey = 0x28;
	UInt32 IncRotKey = 0x27;
	UInt32 DecRotKey = 0x25;
	UInt32 IncScaleKey = 0x21;
	UInt32 DecScaleKey = 0x22;
	UInt32 ShowMenuKey = 0x61;
	UInt32 ClearSelectKey = 0x62;
	UInt32 ResetSelectedNodeKey = 0x63;
};

extern PluginSettings g_pluginSettings;

extern std::unordered_map<UInt32, std::unordered_map<std::string, Nodes::NodeData>> g_modifiedMap;
extern std::unordered_set<UInt32> g_pressedButtons;
extern Actor* g_selectedActor;
extern NiNode* g_selectedNode;