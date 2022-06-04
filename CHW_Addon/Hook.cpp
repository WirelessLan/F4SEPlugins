#include "Global.h"

const uintptr_t cloningWeapon_Offset = 0x332B4;
const uintptr_t handleButtonEvent_Offset = 0x30C32;

uintptr_t cloningWeapon_Target;
uintptr_t handleButtonEvent_Target;

uintptr_t CHWAddress() {
	static uintptr_t chwBaseAddr = (uintptr_t)GetModuleHandle("ClassicHolsteredWeapons.dll");
	return chwBaseAddr;
}

uintptr_t CHWAddress(uintptr_t offset) {
	return CHWAddress() + offset;
}

void Install_CloningWeaponHook() {
	cloningWeapon_Target = CHWAddress(cloningWeapon_Offset);

	uintptr_t	nextInstr = cloningWeapon_Target + 5;
	ptrdiff_t	trampolineDispl = (uintptr_t)Hook_CloningWeapon - nextInstr;

	UInt8	code[5];
	code[0] = 0xE9;
	*((SInt32*)&code[1]) = (SInt32)trampolineDispl;

	SafeWriteBuf(cloningWeapon_Target, code, sizeof(code));
}

void Install_HandleButtonEventHook() {
	handleButtonEvent_Target = CHWAddress(handleButtonEvent_Offset);

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