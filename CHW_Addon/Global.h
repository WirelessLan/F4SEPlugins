#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/xbyak/xbyak.h>
#include <f4se/NiNodes.h>
#include <f4se/GameData.h>
#include <f4se/PapyrusEvents.h>

#include <f4se_common/f4se_version.h>
#include <f4se_common/Relocation.h>
#include <f4se_common/SafeWrite.h>

#include <unordered_map>
#include <fstream>

#include "Hook.h"
#include "Addon.h"
#include "Utility.h"

#define PLUGIN_NAME	"CHW_Addon"
#define PLUGIN_VERSION MAKE_EXE_VERSION(0, 2, 0)