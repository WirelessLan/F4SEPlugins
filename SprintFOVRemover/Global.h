#pragma once
#include <f4se/PluginAPI.h>
#include <f4se/GameReferences.h>
#include <f4se/xbyak/xbyak.h>
#include <f4se_common/f4se_version.h>
#include <f4se_common/Relocation.h>
#include <f4se_common/BranchTrampoline.h>
#include <f4se_common/SafeWrite.h>

#include "ConfigReader.h"

#define PLUGIN_NAME	"SprintFOVRemover"
#define PLUGIN_VERSION MAKE_EXE_VERSION(3, 0, 0)