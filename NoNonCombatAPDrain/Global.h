#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameReferences.h>

#include <f4se_common/f4se_version.h>
#include <f4se_common/SafeWrite.h>

#include "ActorValue.h"
#include "Hooks.h"

#define PLUGIN_NAME	"NoNonCombatAPDrain"
#define PLUGIN_VERSION MAKE_EXE_VERSION(0, 1, 0)

extern RelocPtr<ActorValue> g_actorValue;