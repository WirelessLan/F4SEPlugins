#pragma once

// For asm
extern "C" {
	void AdjustPosition(TESForm* weapForm, NiNode* attachNode);
	void SavePosition();
	extern uintptr_t cloningWeapon_JmpTarget;
	extern uintptr_t handleButtonEvent_JmpTarget;
}

uintptr_t CHWAddress();
uintptr_t CHWAddress(uintptr_t offset);
void Install_Addon();