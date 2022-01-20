#pragma once
#include "Global.h"

typedef void (*_ActorChangeMeshes)(void*, Actor*);
typedef const char* (*_SetModelPath)(void*, UInt64, const char*, const char*);
typedef void* (*_GetNiObject)(NiAVObject*);

void Hooks_ActorChangeMeshes();
void Hooks_SetModelPath();
void Hooks_GetNiObject();