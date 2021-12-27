#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/NiNodes.h>

#include <f4se_common/f4se_version.h>

#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>

#include "TXTReader.h"
#include "CustomModelProcessor.h"
#include "Utils.h"

#define PLUGIN_NAME "CHW_ASS"
#define PLUGIN_VERSION MAKE_EXE_VERSION(1, 1, 0)

extern std::unordered_set<std::string> ActorSkeletonPaths;