#include "Global.h"

bool isSprintQueued = false;

typedef void(*_TogglePOV)(void* arg1, ButtonEvent* event);
RelocAddr<uintptr_t> TogglePOV_FirstToThird_Target(0x2D84518 + 0x40);
RelocAddr<uintptr_t> TogglePOV_ThirdToFirst_Target(0x2D07A38 + 0x40);
_TogglePOV TogglePOV_FirstToThird_Original;
_TogglePOV TogglePOV_ThirdToFirst_Original;

typedef void(*_MovementHandler)(void*, ButtonEvent*);
RelocAddr<uintptr_t> MovementHandler_Target(0x2D49908 + 0x40);
_MovementHandler MovementHandler_Original;

typedef void(*_SprintHandler)(void*, ButtonEvent*);
RelocAddr<uintptr_t> SprintHandler_Target(0x2D499C8 + 0x40);
_SprintHandler SprintHandler_Original;

typedef void(*_ReadyWeaponHandler)(void*, ButtonEvent*);
RelocAddr<uintptr_t> ReadyWeaponHandler_Target(0x2D49B48 + 0x40);
_ReadyWeaponHandler ReadyWeaponHandler_Original;

void TogglePOV_FirstToThird_Hook(void* arg1, ButtonEvent* event) {
	if (IsReloading())
		return;

	TogglePOV_FirstToThird_Original(arg1, event);
}

struct CustomThirdPersonState
{
public:
	UInt32	unk00[0x0EC >> 2];
	float	currentZoomOffset;
};

void TogglePOV_ThirdToFirst_Hook(void* arg1, ButtonEvent* event) {
	static BSFixedString TogglePOV("TogglePOV");
	static BSFixedString ZoomIn("ZoomIn");
	BSFixedString* controlId;

	if (IsReloading()) {
		controlId = event->GetControlID();
		if (*controlId == TogglePOV)
			return;

		if (*controlId == ZoomIn) {
			CustomThirdPersonState* stat = (CustomThirdPersonState*)arg1;
			if (stat && stat->currentZoomOffset == 0.0f)
				return;
		}
	}

	TogglePOV_ThirdToFirst_Original(arg1, event);
}

void MovementHandler_Hook(void* arg1, ButtonEvent* event) {
	static BSFixedString Forward("Forward");

	if (isSprintQueued) {
		if (*(event->GetControlID()) == Forward) {
			if (!IsReloading() && !IsSprinting()) {
				if (IsHoldingButton(event)) 
					ToggleSprint(true);
				isSprintQueued = false;
			}
		}
		else {
			isSprintQueued = false;
		}
	}

	MovementHandler_Original(arg1, event);
}

void SprintHandler_Hook(void* arg1, ButtonEvent* event) {
	if (IsReloading()) {
		isSprintQueued = true;
		return;
	}

	SprintHandler_Original(arg1, event);
}

void ReadyWeaponHandler_Hook(void* arg1, ButtonEvent* event) {
	if (IsWeaponDrawn() && IsThirdPerson() && IsSprinting() && IsButtonPressed(event) && IsWeaponReloadable()) {
		isSprintQueued = true;
		ToggleSprint(false);
	}

	ReadyWeaponHandler_Original(arg1, event);
}

void Install() {
	TogglePOV_FirstToThird_Original = *(_TogglePOV*)(TogglePOV_FirstToThird_Target.GetUIntPtr());
	SafeWrite64(TogglePOV_FirstToThird_Target.GetUIntPtr(), (uintptr_t)TogglePOV_FirstToThird_Hook);

	TogglePOV_ThirdToFirst_Original = *(_TogglePOV*)(TogglePOV_ThirdToFirst_Target.GetUIntPtr());
	SafeWrite64(TogglePOV_ThirdToFirst_Target.GetUIntPtr(), (uintptr_t)TogglePOV_ThirdToFirst_Hook);

	MovementHandler_Original = *(_MovementHandler*)(MovementHandler_Target.GetUIntPtr());
	SafeWrite64(MovementHandler_Target.GetUIntPtr(), (uintptr_t)MovementHandler_Hook);

	SprintHandler_Original = *(_SprintHandler*)(SprintHandler_Target.GetUIntPtr());
	SafeWrite64(SprintHandler_Target.GetUIntPtr(), (uintptr_t)SprintHandler_Hook);

	ReadyWeaponHandler_Original = *(_ReadyWeaponHandler*)(ReadyWeaponHandler_Target.GetUIntPtr());
	SafeWrite64(ReadyWeaponHandler_Target.GetUIntPtr(), (uintptr_t)ReadyWeaponHandler_Hook);
}