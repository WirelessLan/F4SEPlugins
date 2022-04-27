#include "Global.h"

bool isSprintQueued = false;
bool isAutoMoveSprint = false;

RelocPtr<float> minCurrentZoom(0x3805130);

RelocAddr<uintptr_t> TogglePOV_Target(0x2D49E48 + 0x40);
RelocAddr<uintptr_t> TogglePOV_FirstToThird_Target(0x2D84518 + 0x40);
RelocAddr<uintptr_t> TogglePOV_ThirdToFirst_Target(0x2D07A38 + 0x40);
_TogglePOV TogglePOV_Original;
_TogglePOV TogglePOV_FirstToThird_Original;
_TogglePOV TogglePOV_ThirdToFirst_Original;

RelocAddr<uintptr_t> MovementHandler_Target(0x2D49908 + 0x40);
_MovementHandler MovementHandler_Original;

RelocAddr<uintptr_t> SprintHandler_Target(0x2D499C8 + 0x40);
_SprintHandler SprintHandler_Original;

RelocAddr<uintptr_t> ReadyWeaponHandler_Target(0x2D49B48 + 0x40);
_ReadyWeaponHandler ReadyWeaponHandler_Original;

RelocAddr<uintptr_t> PlayerAnimGraphEvent_ReceiveEvent_Target(0x2D442D8 + 0x8);
_PlayerAnimGraphEvent_ReceiveEvent PlayerAnimationEvent_Original;

RelocAddr<uintptr_t> MenuOpenCloseEvent_ReceiveEvent_Target(0x2D49200 + 0x08);
_MenuOpenCloseEvent_ReceiveEvent MenuOpenCloseEvent_ReceiveEvent_Original;

void TogglePOV_Hook(void* arg1, ButtonEvent* event) {
	if (IsReloading())
		return;

	if (IsFirstPerson() && IsWeaponDrawn()) {
		if (!IsVanityModeEnabled())
			ToggleVanityMode(true);
		return;
	}

	TogglePOV_Original(arg1, event);
}

void TogglePOV_FirstToThird_Hook(void* arg1, ButtonEvent* event) {
	if (IsReloading())
		return;

	TogglePOV_FirstToThird_Original(arg1, event);
}

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
			if (stat && minCurrentZoom && stat->currentZoomOffset <= *minCurrentZoom)
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

	if (isAutoMoveSprint)
		isAutoMoveSprint = false;

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
		if (IsAutoMove()) 
			isAutoMoveSprint = true;
		ToggleSprint(false);
	}

	ReadyWeaponHandler_Original(arg1, event);
}

EventResult PlayerAnimGraphEvent_ReceiveEvent_Hook(void* arg1, BSAnimationGraphEvent* evn, void* dispatcher) {
	static BSFixedString ReloadEnd("ReloadEnd");

	if (IsAutoMove() && isAutoMoveSprint) {
		if (evn->name == ReloadEnd) {
			isAutoMoveSprint = false;
			ToggleSprint(true);
		}
	}
	return PlayerAnimationEvent_Original(arg1, evn, dispatcher);
}

EventResult MenuOpenCloseEvent_ReceiveEvent_Hook(void* arg1, MenuOpenCloseEvent* evn, void* dispatcher) {
	static BSFixedString LoadingMenu("LoadingMenu");
	if (evn->menuName == LoadingMenu && evn->isOpen) {
		isSprintQueued = false;
		isAutoMoveSprint = false;
	}

	return MenuOpenCloseEvent_ReceiveEvent_Original(arg1, evn, dispatcher);
}

void Install() {
	TogglePOV_Original = *(_TogglePOV*)(TogglePOV_Target.GetUIntPtr());
	SafeWrite64(TogglePOV_Target.GetUIntPtr(), (uintptr_t)TogglePOV_Hook);

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

	PlayerAnimationEvent_Original = *(_PlayerAnimGraphEvent_ReceiveEvent*)(PlayerAnimGraphEvent_ReceiveEvent_Target.GetUIntPtr());
	SafeWrite64(PlayerAnimGraphEvent_ReceiveEvent_Target, (uintptr_t)PlayerAnimGraphEvent_ReceiveEvent_Hook);

	MenuOpenCloseEvent_ReceiveEvent_Original = *(_MenuOpenCloseEvent_ReceiveEvent*)(MenuOpenCloseEvent_ReceiveEvent_Target.GetUIntPtr());
	SafeWrite64(MenuOpenCloseEvent_ReceiveEvent_Target.GetUIntPtr(), (uintptr_t)MenuOpenCloseEvent_ReceiveEvent_Hook);
}