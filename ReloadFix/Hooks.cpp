#include "Global.h"

RelocPtr<float> minCurrentZoom(0x3805130);

typedef void(*_TogglePOV)(void*, ButtonEvent*);
RelocAddr<uintptr_t> TogglePOV_Target(0x2D49E48 + 0x40);
_TogglePOV TogglePOV_Original;

RelocAddr<uintptr_t> TogglePOV_FirstToThird_Target(0x2D84518 + 0x40);
_TogglePOV TogglePOV_FirstToThird_Original;

RelocAddr<uintptr_t> TogglePOV_ThirdToFirst_Target(0x2D07A38 + 0x40);
_TogglePOV TogglePOV_ThirdToFirst_Original;

typedef void(*_ReadyWeaponHandler)(void*, ButtonEvent*);
RelocAddr<uintptr_t> ReadyWeaponHandler_Target(0x2D49B48 + 0x40);
_ReadyWeaponHandler ReadyWeaponHandler_Original;

typedef EventResult(*_PlayerAnimGraphEvent_ReceiveEvent)(void* arg1, BSAnimationGraphEvent* evn, void* dispatcher);
RelocAddr<uintptr_t> PlayerAnimGraphEvent_ReceiveEvent_Target(0x2D442D8 + 0x8);
_PlayerAnimGraphEvent_ReceiveEvent PlayerAnimGraphEvent_ReceiveEvent_Original;

RelocAddr<uintptr_t> ShouldRestartRelaoding_Target(0xEBB516);

uint32_t g_reloadStackSize;

bool IsReloading() {
	return g_reloadStackSize > 0;
}

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
	if (IsReloading()) {
		if (event->controlID == "TogglePOV")
			return;

		if (event->controlID == "ZoomIn") {
			if (arg1 && minCurrentZoom && ((CustomThirdPersonState*)arg1)->currentZoomOffset <= *minCurrentZoom)
				return;
		}
	}

	TogglePOV_ThirdToFirst_Original(arg1, event);
}

void ReadyWeaponHandler_Hook(void* arg1, ButtonEvent* event) {
	if (IsButtonPressed(event) && IsWeaponDrawn() && IsSprinting() && IsWeaponReloadable() && !IsReloading())
		g_reloadStackSize++;

	ReadyWeaponHandler_Original(arg1, event);
}

EventResult PlayerAnimGraphEvent_ReceiveEvent_Hook(void* arg1, BSAnimationGraphEvent* evn, void* dispatcher) {
	if (evn->name == "reloadState") {
		if (evn->args == "Enter")
			g_reloadStackSize++;
	}
	else if (evn->name == "ReloadComplete" || evn->name == "ReloadEnd" 
		|| evn->name == "weaponDraw" || evn->name == "weaponSheathe") {
		g_reloadStackSize = 0;
	}

	return PlayerAnimGraphEvent_ReceiveEvent_Original(arg1, evn, dispatcher);
}

void Install() {
	TogglePOV_Original = *(_TogglePOV*)(TogglePOV_Target.GetUIntPtr());
	SafeWrite64(TogglePOV_Target.GetUIntPtr(), (uintptr_t)TogglePOV_Hook);

	TogglePOV_FirstToThird_Original = *(_TogglePOV*)(TogglePOV_FirstToThird_Target.GetUIntPtr());
	SafeWrite64(TogglePOV_FirstToThird_Target.GetUIntPtr(), (uintptr_t)TogglePOV_FirstToThird_Hook);

	TogglePOV_ThirdToFirst_Original = *(_TogglePOV*)(TogglePOV_ThirdToFirst_Target.GetUIntPtr());
	SafeWrite64(TogglePOV_ThirdToFirst_Target.GetUIntPtr(), (uintptr_t)TogglePOV_ThirdToFirst_Hook);

	ReadyWeaponHandler_Original = *(_ReadyWeaponHandler*)(ReadyWeaponHandler_Target.GetUIntPtr());
	SafeWrite64(ReadyWeaponHandler_Target.GetUIntPtr(), (uintptr_t)ReadyWeaponHandler_Hook);

	PlayerAnimGraphEvent_ReceiveEvent_Original = *(_PlayerAnimGraphEvent_ReceiveEvent*)(PlayerAnimGraphEvent_ReceiveEvent_Target.GetUIntPtr());
	SafeWrite64(PlayerAnimGraphEvent_ReceiveEvent_Target.GetUIntPtr(), (uintptr_t)PlayerAnimGraphEvent_ReceiveEvent_Hook);

	uint8_t buf[] = { 0x40, 0x30, 0xFF, 0x90 };	// xor dil, dil; nop;
	SafeWriteBuf(ShouldRestartRelaoding_Target.GetUIntPtr(), buf, sizeof(buf));
}