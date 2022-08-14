#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameData.h>
#include <f4se/GameMenus.h>
#include <f4se/GameReferences.h>
#include <f4se/GameRTTI.h>
#include <f4se/PapyrusNativeFunctions.h>
#include <f4se/PapyrusEvents.h>
#include <f4se/ScaleformLoader.h>

#include <f4se_common/f4se_version.h>

#include <common/IDirectoryIterator.h>

#include <fstream>
#include <vector>
#include <algorithm>

#include "SlotManager.h"
#include "ScaleformManager.h"
#include "Util.h"

#define PLUGIN_NAME	"CoordiSaver"
#define PLUGIN_VERSION MAKE_EXE_VERSION(0, 5, 0)