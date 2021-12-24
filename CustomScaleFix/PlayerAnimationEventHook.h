#pragma once

#include "f4se/GameEvents.h"

class BSAnimationGraphEvent {
public:
	// members
	UInt64				unk00;
	BSFixedString		name;
};

void Hooks_PlayerAnimationEvent();