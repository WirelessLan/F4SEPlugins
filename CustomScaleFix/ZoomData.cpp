#include "ZoomData.h"

#include <f4se/GameForms.h>
#include <f4se/GameObjects.h>
#include <f4se/GameReferences.h>
#include <f4se/xbyak/xbyak.h>
#include <f4se_common/BranchTrampoline.h>

#include "Utils.h"

namespace ZoomData {
	class ZoomData : public TESForm {
	public:
		struct Data {
		public:
			float fovMult;              // 00
			std::uint32_t overlay;      // 04
			std::uint32_t isModFormID;  // 08
			NiPoint3 cameraOffset;      // 0C
		};

		Data zoomData;                  // 20
		TESImageSpaceModifier* isMod;   // 38
	};

	NiPoint3* GetZoomData(TESObjectWEAP::InstanceData* arg1) {
		static NiPoint3 cameraOffset = {};

		ZoomData* zoomData = reinterpret_cast<ZoomData*>(arg1->zoomData);
		if (!zoomData)
			return nullptr;

		cameraOffset = zoomData->zoomData.cameraOffset;
		cameraOffset *= Utils::GetScale(*g_player);

		return &cameraOffset;
	}

	void Hooks_GetGeometryOffset() {
		struct asm_code : Xbyak::CodeGenerator {
			asm_code(void* a_buf, std::uintptr_t a_funcAddr, std::uintptr_t a_targetAddr) : Xbyak::CodeGenerator(4096, a_buf) {
				Xbyak::Label funcLabel;
				Xbyak::Label jmpLabel;
				Xbyak::Label retnLabel;

				push(rbx);
				push(rcx);
				push(rdx);
				push(rdi);
				sub(rsp, 0x10); movdqu(ptr[rsp], xmm6);

				call(ptr[rip + funcLabel]);

				movdqu(xmm6, ptr[rsp]); add(rsp, 0x10);
				pop(rdi);
				pop(rdx);
				pop(rcx);
				pop(rbx);

				test(rax, rax);
				je("RETURN");

				movss(xmm4, ptr[rax + 0x4]);
				movss(xmm5, ptr[rax + 0x8]);
				movaps(ptr[rsp + 0x50], xmm6);
				movaps(xmm1, xmm5);
				movaps(xmm2, xmm4);
				movss(xmm6, ptr[rax]);
				jmp(ptr[rip + jmpLabel]);

				L("RETURN");
				jmp(ptr[rip + retnLabel]);

				L(funcLabel);
				dq(a_funcAddr);

				L(retnLabel);
				dq(a_targetAddr + 0x13);

				L(jmpLabel);
				dq(a_targetAddr + 0x33);
			}
		};

		RelocAddr<std::uintptr_t> target(0x12444E6);

		void* codeBuf = g_localTrampoline.StartAlloc();
		asm_code code(codeBuf, (std::uintptr_t)GetZoomData, target.GetUIntPtr());
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write6Branch(target.GetUIntPtr(), (std::uintptr_t)code.getCode());
	}
}