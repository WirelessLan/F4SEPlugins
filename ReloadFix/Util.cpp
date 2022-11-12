#include "Global.h"

bool IsSprinting() {
	if (!*g_player)
		return false;

	return (*g_player)->actorState.unk08 & 0x0100;
}

bool IsFirstPerson() {
	if (!*g_playerCamera)
		return false;

	return (*g_playerCamera)->cameraState == (*g_playerCamera)->cameraStates[PlayerCamera::kCameraState_FirstPerson];
}

bool IsWeaponDrawn() {
	if (!*g_player)
		return false;

	return (*g_player)->actorState.IsWeaponDrawn();
}

bool IsVanityModeEnabled() {
	if (!*g_playerControls)
		return false;

	return (*g_playerControls)->unk88 & 0x1000000;
}

void ToggleVanityMode(bool enable) {
	if (!*g_playerControls)
		return;

	if (enable)
		(*g_playerControls)->unk88 |= 0x1000000;
	else
		(*g_playerControls)->unk88 &= ~0x1000000;
}

struct LOAD_DATA {
	struct Data {
		UInt64 unk00;
		Actor* actor;
	};

	void* arg1;
	void* arg2;
};

typedef bool (*_FullyLoaded)(LOAD_DATA*, Actor::MiddleProcess::Data08::EquipData*);
RelocAddr <_FullyLoaded> FullyLoaded_Internal(0xE24B80);

bool IsWeaponReloadable() {
	if (!*g_player || !(*g_player)->middleProcess || !(*g_player)->middleProcess->unk08)
		return false;

	tArray<Actor::MiddleProcess::Data08::EquipData> equipDataArr = reinterpret_cast<tArray<Actor::MiddleProcess::Data08::EquipData> &>((*g_player)->middleProcess->unk08->equipData);
	if (equipDataArr.count == 0)
		return false;

	Actor::MiddleProcess::Data08::EquipData* equipData = nullptr;
	for (UInt32 ii = 0; ii < equipDataArr.count; ii++) {
		UInt32 equipIndex = *reinterpret_cast<UInt32*>(&equipDataArr.entries[ii].unk18);
		if (equipIndex == 0) {
			equipData = &equipDataArr.entries[ii];
			break;
		}
	}
	if (!equipData)
		return false;

	LOAD_DATA::Data data = { 0 };
	data.actor = *g_player;

	LOAD_DATA loadData = { &data.unk00, &data.actor };

	return !FullyLoaded_Internal(&loadData, equipData);
}

bool IsButtonPressed(ButtonEvent* btnEvent) {
	if (btnEvent->isDown == BUTTON_UP && (btnEvent->timer > 0 && btnEvent->timer < BUTTON_HOLD_TIMER))
		return true;
	return false;
}