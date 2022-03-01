#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameData.h>
#include <f4se/GameRTTI.h>
#include <f4se/GameEvents.h>
#include <f4se/GameReferences.h>
#include <f4se/NiNodes.h>
#include <f4se/xbyak/xbyak.h>

#include <f4se_common/f4se_version.h>
#include <f4se_common/BranchTrampoline.h>
#include <f4se_common/SafeWrite.h>

#include "Util.h"
#include "ActorScaleHooks.h"
#include "PlayerAnimationEventHook.h"
#include "SkeletonManager.h"
#include "AAFSupport.h"
#include "ConfigReader.h"

#define PLUGIN_NAME	"CustomScaleFix"
#define PLUGIN_VERSION	MAKE_EXE_VERSION(2, 4, 1)

struct PluginOptions {
	bool useCameraOverrideScaleFix = true;
	bool useUnifyAAFDoppelgangerScale = true;
};

extern PluginOptions			g_pluginOptions;