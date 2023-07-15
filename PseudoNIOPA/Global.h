#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameReferences.h>
#include <f4se/GameObjects.h>
#include <f4se/GameExtraData.h>
#include <f4se/GameRTTI.h>
#include <f4se/GameInput.h>
#include <f4se/GameMenus.h>
#include <f4se/GameSettings.h>
#include <f4se/InputMap.h>
#include <f4se/NiNodes.h>
#include <f4se/PapyrusNativeFunctions.h>
#include <f4se/ScaleformLoader.h>
#include <f4se/ScaleformCallbacks.h>

#include <f4se_common/f4se_version.h>
#include <f4se_common/SafeWrite.h>

#include <common/IDirectoryIterator.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>

#include "PapyrusFuncs.h"
#include "ScaleformManager.h"
#include "InputEnableManager.h"
#include "Nodes.h"
#include "Inputs.h"
#include "Events.h"
#include "Utility.h"
#include "ConfigReader.h"

#define PLUGIN_NAME	"PseudoNIOPA"
#define PLUGIN_VERSION MAKE_EXE_VERSION(1, 2, 0)

struct PluginSettings {
	std::uint32_t DirXKey = 0xBC;
	std::uint32_t DirYKey = 0xBE;
	std::uint32_t DirZKey = 0xBF;
	std::uint32_t IncTransKey = 0x26;
	std::uint32_t DecTransKey = 0x28;
	std::uint32_t IncRotKey = 0x27;
	std::uint32_t DecRotKey = 0x25;
	std::uint32_t IncScaleKey = 0x21;
	std::uint32_t DecScaleKey = 0x22;
};

extern PluginSettings g_pluginSettings;

extern Actor* g_selectedActor;
extern NiNode* g_selectedNode;
extern std::unordered_map<std::uint32_t, std::unordered_map<std::string, Nodes::NodeData>> g_modifiedMap;