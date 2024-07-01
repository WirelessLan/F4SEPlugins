#include "BSTextureIndex.h"

#include <f4se/xbyak/xbyak.h>
#include <f4se_common/BranchTrampoline.h>
#include <f4se_common/SafeWrite.h>

#define	MAX_SIZE	4096

namespace BSTextureIndex {
	BSResource::ID g_unk38[MAX_SIZE];

	void Overwrites_Instruction() {
		RelocAddr<std::uintptr_t> target(0x1CB89A6);
		// mov r9d, r12d; nop;
		unsigned char buf[] = { 0x45, 0x8B, 0xCC , 0x90 };
		SafeWriteBuf(target.GetUIntPtr(), buf, sizeof(buf));
	}

	void Hooks_AddDataFile() {
		struct asm_code : Xbyak::CodeGenerator {
			asm_code(void* a_buf, std::uintptr_t a_target) : Xbyak::CodeGenerator(4096, a_buf) {
				Xbyak::Label retnLabel;

				mov(rbx, (std::uintptr_t)g_unk38);

				mov(eax, ptr[rsi + 0x8]);
				lea(rdx, ptr[rdi + rdi * 0x2]);
				mov(ptr[rbx + rdx * 0x4 + 0x8], eax);
				mov(ecx, ptr[rsi + 0x4]);
				mov(eax, ptr[rsi]);
				shl(rcx, 0x20);
				or(rcx, rax);
				mov(ptr[rbx + rdx * 0x4], ecx);
				shr(rcx, 0x20);
				mov(ptr[rbx + rdx * 0x4 + 0x4], ecx);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(a_target + 0x23);
			}
		};

		RelocAddr<std::uintptr_t> target(0x1CB4087);

		void* codeBuf = g_localTrampoline.StartAlloc();
		asm_code code(codeBuf, target.GetUIntPtr());
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(target.GetUIntPtr(), (std::uintptr_t)codeBuf);
	}
}