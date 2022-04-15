#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameReferences.h>
#include <f4se/GameData.h>
#include <f4se/GameRTTI.h>
#include <f4se/NiExtraData.h>
#include <f4se/xbyak/xbyak.h>

#include <f4se_common/f4se_version.h>
#include <f4se_common/Relocation.h>
#include <f4se_common/BranchTrampoline.h>
#include <f4se_common/SafeWrite.h>

#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <thread>

#include "CACS.h"
#include "Hooks.h"
#include "Utils.h"

#define PLUGIN_NAME	"CustomMeshesPathF4"
#define PLUGIN_VERSION	MAKE_EXE_VERSION(0, 6, 1)