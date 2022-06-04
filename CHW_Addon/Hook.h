#pragma once

// Asm Functions
extern "C" {
	void Hook_CloningWeapon();
	void Hook_HandleButtonEvent();

	// For Hook code
	void AdjustPosition(TESForm* weapForm, NiNode* attachNode);
	void SavePosition();
	extern uintptr_t cloningWeapon_Target;
	extern uintptr_t handleButtonEvent_Target;
}

uintptr_t CHWAddress();
uintptr_t CHWAddress(uintptr_t offset);
void Install_Addon();