#include "Global.h"

uintptr_t cloningWeapon_Offset = 0x332B4;
uintptr_t handleButtonEvent_Offset = 0x30C32;

uintptr_t CHWAddress() {
	static uintptr_t chwBaseAddr = (uintptr_t)GetModuleHandle("ClassicHolsteredWeapons.dll");
	return chwBaseAddr;
}

uintptr_t CHWAddress(uintptr_t offset) {
	return CHWAddress() + offset;
}

void Install_Addon() {
	if (!CHWAddress(0)) {
		_MESSAGE("Cannot find ClassicHolsteredWeapons.dll!");
		return;
	}

	_MESSAGE("CHW BaseAddr: %016I64X", CHWAddress());

	if (!g_branchTrampoline.Create(1024 * 64, (void*)CHWAddress())) {
		_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
		return;
	}

	if (!g_localTrampoline.Create(1024 * 64, (void*)CHWAddress())) {
		_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
		return;
	}

	{
		struct CloningWeapon_Code : Xbyak::CodeGenerator {
			CloningWeapon_Code(void* buf, UInt64 targetAddr) : Xbyak::CodeGenerator(4096, buf) {
				Xbyak::Label retLabel;

				mov(rcx, ptr[rsp + 0x40]);
				mov(rdx, ptr[rsp + 0xB0]);
				call(AdjustPosition);

				cmp(qword[rsp + 0x30], 0x00);
				jmp(ptr[rip + retLabel]);

				L(retLabel);
				dq(targetAddr + 0x06);
			}
		};

		void* codeBuf = g_localTrampoline.StartAlloc();
		CloningWeapon_Code code(codeBuf, CHWAddress(cloningWeapon_Offset));
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write6Branch(CHWAddress(cloningWeapon_Offset), (uintptr_t)code.getCode());
	}

	{
		struct HandleButtonEvent_Code : Xbyak::CodeGenerator {
			HandleButtonEvent_Code(void* buf, UInt64 targetAddr) : Xbyak::CodeGenerator(4096, buf) {
				Xbyak::Label retLabel;

				cmp(dword[rsp + 0x20], 0x65);
				jne("DEFAULT");
				call(SavePosition);

				L("DEFAULT");
				cmp(dword[rsp + 0x20], 0x6B);
				jmp(ptr[rip + retLabel]);

				L(retLabel);
				dq(targetAddr + 0x05);
			}
		};

		void* codeBuf = g_localTrampoline.StartAlloc();
		HandleButtonEvent_Code code(codeBuf, CHWAddress(handleButtonEvent_Offset));
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(CHWAddress(handleButtonEvent_Offset), (uintptr_t)code.getCode());
	}

	_MESSAGE("CHW_Addon Installed!");
}