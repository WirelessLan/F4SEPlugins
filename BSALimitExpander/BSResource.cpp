#include "BSResource.h"

#include <f4se/NiTypes.h>
#include <f4se/xbyak/xbyak.h>
#include <f4se_common/BranchTrampoline.h>

#define	MAX_SIZE	4096

namespace BSResource {
	Stream* g_unk30[MAX_SIZE];
	void* g_unk830[MAX_SIZE];
	ID g_unk1030[MAX_SIZE];

	namespace Archive2 {
		void Func_125260(Stream*& a_stream, NiPointer<Stream>& a_res) {
			using func_t = decltype(&Func_125260);
			func_t func = RelocAddr<func_t>(0x125260);
			func(a_stream, a_res);
		}

		std::uint64_t* Func_1B16AB0(std::uint64_t a_arg1) {
			using func_t = decltype(&Func_1B16AB0);
			func_t func = RelocAddr<func_t>(0x1B16AB0);
			return func(a_arg1);
		}

		bool Func_1B772F0(std::uint64_t& a_arg1, std::uint32_t a_index, ID*& a_id) {
			using func_t = decltype(&Func_1B772F0);
			func_t func = RelocAddr<func_t>(0x1B772F0);
			return func(a_arg1, a_index, a_id);
		}

		void Func_1B7B740(std::uint64_t& a_arg1, ID*& a_id) {
			using func_t = decltype(&Func_1B7B740);
			func_t func = RelocAddr<func_t>(0x1B7B740);
			func(a_arg1, a_id);
		}

		void AddDataFile(Index& a_self, NiPointer<Stream>& a_res, ID& a_id, std::uint32_t a_index) {
			Func_125260(g_unk30[a_index], a_res);
			a_res->Unk_10(g_unk830[a_index]);

			if (a_self.unk1C30 != a_index)
				return;

			g_unk1030[a_index] = a_id;
			ID* p_id = g_unk1030;

			if (a_self.unk18 == RelocAddr<std::uint64_t*>(0x3845978)) {
				a_self.unk28 = 2;
				a_self.unk18 = Func_1B16AB0(2);
			}
			else {
				bool result = Func_1B772F0(a_self.unk10, a_index, p_id);
				if (!result)
					Func_1B7B740(a_self.unk10, p_id);
			}

			Func_1B772F0(a_self.unk10, a_index, p_id);
			a_self.unk1C30++;
		}

		void Hooks_AddDataFile() {
			RelocAddr<std::uintptr_t> target(0x1B7A346);
			g_branchTrampoline.Write5Call(target.GetUIntPtr(), (std::uintptr_t)AddDataFile);
		}
	}

	namespace SDirectory2 {
		void Hooks_Retrieve() {
			{
				struct asm_code : Xbyak::CodeGenerator {
					asm_code(void* a_buf, std::uintptr_t a_target) : Xbyak::CodeGenerator(4096, a_buf) {
						Xbyak::Label retnLabel;

						mov(rcx, (std::uintptr_t)g_unk30);
						mov(rdx, ptr[rcx + rax * 8]);

						jmp(ptr[rip + retnLabel]);

						L(retnLabel);
						dq(a_target + 0x5);
					}
				};

				RelocAddr<std::uintptr_t> target(0x1B71BD6);

				void* codeBuf = g_localTrampoline.StartAlloc();
				asm_code code(codeBuf, target.GetUIntPtr());
				g_localTrampoline.EndAlloc(code.getCurr());

				g_branchTrampoline.Write5Branch(target.GetUIntPtr(), (std::uintptr_t)codeBuf);
			}

			{
				struct asm_code : Xbyak::CodeGenerator {
					asm_code(void* a_buf, std::uintptr_t a_target) : Xbyak::CodeGenerator(4096, a_buf) {
						Xbyak::Label retnLabel;

						mov(rcx, (std::uintptr_t)g_unk830);
						mov(rdx, ptr[rcx + rax * 8]);

						jmp(ptr[rip + retnLabel]);

						L(retnLabel);
						dq(a_target + 0x8);
					}
				};

				RelocAddr<std::uintptr_t> target(0x1B728B5);

				void* codeBuf = g_localTrampoline.StartAlloc();
				asm_code code(codeBuf, target.GetUIntPtr());
				g_localTrampoline.EndAlloc(code.getCurr());

				g_branchTrampoline.Write5Branch(target.GetUIntPtr(), (std::uintptr_t)codeBuf);
			}
		}
	}
}