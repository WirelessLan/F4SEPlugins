#pragma once
#include "f4se/GameData.h"
#include "f4se/GameRTTI.h"
#include "f4se/GameEvents.h"

#include "f4se_common/f4se_version.h"

#include "Util.h"
#include "ActorScaleHooks.h"
#include "PlayerAnimationEventHook.h"
#include "SkeletonManager.h"
#include "AAFSupport.h"
#include "ConfigReader.h"

#define PLUGIN_NAME	"PedoHeightFix"
#define PLUGIN_VERSION	MAKE_EXE_VERSION(2, 3, 1)

struct PluginOptions {
	bool useDynamicVerticalCollision = true;
	bool useCameraOverrideHeightFix = true;
	bool useUnifyAAFDoppelgangerScale = true;
};

extern PluginOptions			g_pluginOptions;