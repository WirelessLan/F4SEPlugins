#pragma once

struct CustomThirdPersonState
{
public:
	UInt32	unk00[0x0EC >> 2];
	float	currentZoomOffset;
};

class BSAnimationGraphEvent {
public:
	TESObjectREFR* refr;
	BSFixedString	name;
	BSFixedString	args;
};

typedef EventResult(*_PlayerAnimGraphEvent_ReceiveEvent)(void*, BSAnimationGraphEvent*, void*);

void Install();