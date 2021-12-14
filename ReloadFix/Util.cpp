#include "Global.h"

bool IsReloading() {
	if (!*g_player)
		return false;

	UInt32 reload_flag = ((*g_player)->actorState.flags >> 14) & 0xF;
	return reload_flag == 0x04;
}

bool IsWeaponReloadable() {
	if (!*g_player || !(*g_player)->middleProcess || !(*g_player)->middleProcess->unk08)
		return false;

	tArray<Actor::MiddleProcess::Data08::EquipData> equipData = reinterpret_cast<tArray<Actor::MiddleProcess::Data08::EquipData> &>((*g_player)->middleProcess->unk08->equipData);
	if (equipData.count == 0)
		return false;

	EquippedWeaponData* equipedWeap = equipData.entries[0].equippedData;
	if (!equipedWeap || !equipedWeap->ammo)
		return false;

	TESObjectWEAP::InstanceData* weapInst = (TESObjectWEAP::InstanceData*)equipData.entries[0].instanceData;
	if (!weapInst) {
		TESObjectWEAP* objWeap = DYNAMIC_CAST(equipData.entries[0].item, TESForm, TESObjectWEAP);
		if (!objWeap)
			return false;

		weapInst = &objWeap->weapData;
	}

	UInt32 loadedAmmoCount = static_cast<UInt32>(equipedWeap->unk18);
	if (loadedAmmoCount == weapInst->ammoCapacity)
		return false;

	BGSInventoryList* playerInventory = (*g_player)->inventoryList;
	if (!playerInventory)
		return false;

	UInt32 totalAmmoCount = 0;

	playerInventory->inventoryLock.LockForRead();
	for (UInt32 ii = 0; ii < playerInventory->items.count; ii++) {
		if (playerInventory->items[ii].form == equipedWeap->ammo) {
			if (playerInventory->items[ii].stack)
				totalAmmoCount = playerInventory->items[ii].stack->count;
			break;
		}
	}
	playerInventory->inventoryLock.Unlock();

	if (totalAmmoCount == 0)
		return false;

	if (totalAmmoCount <= loadedAmmoCount)
		return false;

	return true;
}

bool IsSprinting() {
	if (!*g_player)
		return false;

	return (*g_player)->actorState.unk08 & 0x0100;
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

#define BUTTON_UP 0.0f
#define BUTTON_DOWN 1.0f
#define BUTTON_HOLD_TIMER 0.4f

bool IsButtonPressed(ButtonEvent* btnEvent) {
	if (btnEvent->isDown == BUTTON_UP && (btnEvent->timer > 0 && btnEvent->timer < BUTTON_HOLD_TIMER))
		return true;
	return false;
}

bool IsHoldingButton(ButtonEvent* btnEvent) {
	if (btnEvent->isDown == BUTTON_DOWN && btnEvent->timer >= BUTTON_HOLD_TIMER)
		return true;
	return false;
}

void ToggleSprint(bool a_sprint) {
	if (a_sprint) {
		UInt64 val = 8;
		(*g_player)->unkC90[31] |= val;
	}
	else {
		UInt64 val = ~8;
		(*g_player)->unkC90[31] &= val;
	}
}