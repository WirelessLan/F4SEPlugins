#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameData.h>
#include <f4se/GameCamera.h>
#include <f4se/GameReferences.h>
#ifdef _HOOK_FORCEAV
#include <f4se/xbyak/xbyak.h>
#endif

#include <f4se_common/f4se_version.h>
#include <f4se_common/SafeWrite.h>
#ifdef _HOOK_FORCEAV
#include <f4se_common/BranchTrampoline.h>
#endif

#include "Util.h"
#include "Hooks.h"
#include "ActorValueInfo.h"

#define PLUGIN_NAME	"ThirdPersonSpeedMultFix"
#define PLUGIN_VERSION MAKE_EXE_VERSION(0, 4, 0)

extern RelocPtr <ActorValue> g_actorValue;

extern ModifiedValues g_modifiedSpeedMult;