#include "Global.h"

const uintptr_t cloningWeapon_Offset = 0x332B4;
const uintptr_t handleButtonEvent_Offset = 0x30C32;

uintptr_t cloningWeapon_JmpTarget;
uintptr_t handleButtonEvent_JmpTarget;

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
	uintptr_t cloningWeapon_Target = CHWAddress(cloningWeapon_Offset);
	cloningWeapon_JmpTarget = cloningWeapon_Target + 0x06;

	uintptr_t	nextInstr = cloningWeapon_Target + 5;
	ptrdiff_t	trampolineDispl = (uintptr_t)Hook_CloningWeapon - nextInstr;

	UInt8	code[5];
	code[0] = 0xE9;
	*((SInt32*)&code[1]) = (SInt32)trampolineDispl;

	SafeWriteBuf(cloningWeapon_Target, code, sizeof(code));
}

void Install_HandleButtonEventHook() {
	uintptr_t handleButtonEvent_Target = CHWAddress(handleButtonEvent_Offset);
	handleButtonEvent_JmpTarget = handleButtonEvent_Target + 0x05;

	uintptr_t	nextInstr = handleButtonEvent_Target + 5;
	ptrdiff_t	trampolineDispl = (uintptr_t)Hook_HandleButtonEvent - nextInstr;

	UInt8	code[5];
	code[0] = 0xE9;
	*((SInt32*)&code[1]) = (SInt32)trampolineDispl;

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