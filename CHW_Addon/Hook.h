#pragma once

// For asm
extern "C" {
	void AdjustPosition(TESForm* weapForm, NiNode* attachNode);
	void SavePosition();
	extern uintptr_t cloningWeapon_Target;
	extern uintptr_t handleButtonEvent_Target;
	extern unsigned char SavePositionKey;
}

uintptr_t CHWAddress();
uintptr_t CHWAddress(uintptr_t offset);
void Install_Addon();