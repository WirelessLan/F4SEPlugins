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

typedef void(*_TogglePOV)(void* arg1, ButtonEvent* event);
typedef void(*_MovementHandler)(void*, ButtonEvent*);
typedef void(*_SprintHandler)(void*, ButtonEvent*);
typedef void(*_ReadyWeaponHandler)(void*, ButtonEvent*);
typedef EventResult(*_PlayerAnimGraphEvent_ReceiveEvent)(void*, BSAnimationGraphEvent*, void*);
typedef EventResult(*_MenuOpenCloseEvent_ReceiveEvent)(void*, MenuOpenCloseEvent*, void*);

void Install();