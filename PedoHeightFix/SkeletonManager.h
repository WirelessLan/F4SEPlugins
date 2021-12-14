#pragma once

#include "f4se/GameReferences.h"
#include "f4se/NiExtraData.h"
#include "f4se/NiNodes.h"

class BSBound : public NiExtraData {
public:
	// members
	NiPoint3 center;   // 18
	NiPoint3 extents;  // 24
};

class SkeletonManager {
public:
	static void ModifyScale(Actor *actor, bool isFirstPerson, float scale);
	static void UpdateBBX(Actor* actor, float scale);
};