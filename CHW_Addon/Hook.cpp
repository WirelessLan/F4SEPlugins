#include "Global.h"

const uintptr_t cloningWeapon_Offset = 0x332B4;
const uintptr_t handleButtonEvent_Offset = 0x30C32;

uintptr_t cloningWeapon_Target;
uintptr_t handleButtonEvent_Target;

extern "C" {
	void Hook_CloningWeapon();
	void Hook_HandleButtonEvent();
}

uintptr_t CHWAddress() {
	static uintptr_t chwBaseAddr = (uintptr_t)GetModuleHandle("ClassicHolsteredWeapons.dll");
	return chwBaseAddr;
}

uintptr_t CHWAddress(uintptr_t offset) {
	return CHWAddress() + offset;
}

void Install_CloningWeaponHook() {
	cloningWeapon_Target = CHWAddress(cloningWeapon_Offset);

	UInt8	code[12];
	code[0] = 0x48;
	code[1] = 0xb8;
	*((uintptr_t*)&code[2]) = (uintptr_t)Hook_CloningWeapon;
	code[10] = 0xff;
	code[11] = 0xe0;

	SafeWriteBuf(cloningWeapon_Target, code, sizeof(code));
}

void Install_HandleButtonEventHook() {
	handleButtonEvent_Target = CHWAddress(handleButtonEvent_Offset);

	UInt8	code[12];
	code[0] = 0x48;
	code[1] = 0xb8;
	*((uintptr_t*)&code[2]) = (uintptr_t)Hook_HandleButtonEvent;
	code[10] = 0xff;
	code[11] = 0xe0;

	SafeWriteBuf(handleButtonEvent_Target, code, sizeof(code));
}

void Install_Addon() {
	if (!CHWAddress(0)) {
		_MESSAGE("Cannot find ClassicHolsteredWeapons.dll!");
		return;
	}

	_MESSAGE("CHW BaseAddr: %016I64X", CHWAddress());

	Install_CloningWeaponHook();
	Install_HandleButtonEventHook();

	_MESSAGE("CHW_Addon Installed!");
}