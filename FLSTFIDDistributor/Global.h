#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameRTTI.h>
#include <f4se/GameData.h>

#include <f4se_common/f4se_version.h>

#include <common/IDirectoryIterator.h>

#include <fstream>
#include <unordered_map>

#include "Distributor.h"
#include "Utility.h"

#define PLUGIN_NAME	"FLSTFIDDistributor"
#define PLUGIN_VERSION MAKE_EXE_VERSION(0, 1, 0)