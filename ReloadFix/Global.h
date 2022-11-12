#pragma once
#include <f4se/GameReferences.h>
#include <f4se/GameCamera.h>

#include "f4se_common/f4se_version.h"
#include "f4se_common/SafeWrite.h"

#include "Util.h"
#include "Hooks.h"

#define PLUGIN_NAME	"ReloadFix"
#define PLUGIN_VERSION MAKE_EXE_VERSION(0, 7, 0)

extern uint32_t g_reloadStackSize;