#include "Global.h"

bool IsSprinting() {
	if (!*g_player)
		return false;

	return (*g_player)->actorState.unk08 & 0x0100;
}

bool IsSneaking() {
	if (!*g_player)
		return false;

	UInt32 stance = ((*g_player)->actorState.flags >> 11) & 0x7;
	return stance == 1;
}

bool IsMoving() {
	if (!*g_player)
		return false;

	return (*g_player)->actorState.unk08 & 0x86;
}

bool IsThirdPerson() {
	if (!*g_playerCamera)
		return false;

	return (*g_playerCamera)->cameraState == (*g_playerCamera)->cameraStates[PlayerCamera::kCameraState_ThirdPerson2];
}

bool IsWeaponDrawn() {
	if (!*g_player)
		return false;

	return (*g_player)->actorState.IsWeaponDrawn();
}