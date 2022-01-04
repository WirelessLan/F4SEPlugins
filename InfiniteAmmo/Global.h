#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameEvents.h>
#include <f4se/GameReferences.h>
#include <f4se/GameObjects.h>
#include <f4se/GameExtraData.h>
#include <f4se/GameRTTI.h>
#include <f4se/GameData.h>

#include <f4se_common/f4se_version.h>
#include <f4se_common/Relocation.h>
#include <f4se_common/SafeWrite.h>

#include <fstream>
#include <sstream>
#include <unordered_set>

#include "Events.h"
#include "Hooks.h"
#include "Utils.h"
#include "InfiniteAmmo.h"

#define PLUGIN_NAME	"InfiniteAmmo"
#define PLUGIN_VERSION MAKE_EXE_VERSION(1, 3, 2)

extern UInt16 CurrentAmmoCapacity;