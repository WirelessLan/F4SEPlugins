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

#include <vector>
#include <fstream>

#include "Util.h"
#include "ZoomData.h"
#include "FurniturePosition.h"
#include "PlayerAnimationEvent.h"
#include "ConfigReader.h"

#define PLUGIN_NAME	"CustomScaleFix"
#define PLUGIN_VERSION	MAKE_EXE_VERSION(2, 6, 0)

enum FurniturePositionType {
	kFurniturePositionType_Relative = 1,
	kFurniturePositionType_Absolute = 2
};

struct PluginOptions {
	bool useCameraOverrideScaleFix = true;
	bool useUnifyAAFDoppelgangerScale = true;
	bool useCustomFurniturePosition = true;
	bool playerOnlyCustomFurniturePosition = false;
	UInt32 furniturePositionType = FurniturePositionType::kFurniturePositionType_Relative;
};

extern PluginOptions			g_pluginOptions;