#pragma once

struct IsReloadableData {
	UInt64 unk00;
	Actor* actor;
};

struct IsReloadableDataWrapper {
	void* arg1;
	void* arg2;
};

bool IsReloading();
bool IsSprinting();
bool IsFirstPerson();
bool IsThirdPerson();
bool IsWeaponDrawn();
bool IsAutoMove();

class FirstPersonState : public TESCameraState {
public:
	UInt64	unk28[(0x80 - 0x28) >> 3];
	bool	unk80;
	bool	unk81;
	bool	unk82;
	bool	unk83;
};

bool IsPreventTogglePOVEnabled();

typedef bool (*_IsReloadable)(IsReloadableDataWrapper*, Actor::MiddleProcess::Data08::EquipData*);
bool IsWeaponReloadable();

#define BUTTON_UP 0.0f
#define BUTTON_DOWN 1.0f
#define BUTTON_HOLD_TIMER 0.4f

bool IsButtonPressed(ButtonEvent* btnEvent);
bool IsHoldingButton(ButtonEvent* btnEvent);

void ToggleSprint(bool a_sprint);