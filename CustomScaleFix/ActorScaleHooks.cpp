#include "Global.h"

typedef NiPoint3* (*_GetZoomData)(BGSZoomData*);
RelocAddr <_GetZoomData> GetZoomData_Target(0x12444E6);

NiPoint3* GetModifiedZoomData(TESObjectWEAP::InstanceData* arg1) {
	static NiPoint3 cameraOffset = {};

	if (!arg1->zoomData)
		return nullptr;

	cameraOffset = arg1->zoomData->zoomData.cameraOffset;
	cameraOffset *= GetScale(*g_player);

	return &cameraOffset;
}

void Hooks_GetZoomData() {
	struct AiProcess_Code : Xbyak::CodeGenerator {
		AiProcess_Code(void* buf, UInt64 funcAddr, UInt64 targetAddr) : Xbyak::CodeGenerator(4096, buf)	{
			Xbyak::Label funcLabel;
			Xbyak::Label jmpLabel;
			Xbyak::Label retnLabel;

			push(rbx);
			push(rcx);
			push(rdx);
			push(rdi);
			sub(rsp, 0x10);
			movdqu(ptr[rsp], xmm6);

			call(ptr[rip + funcLabel]);

			movdqu(xmm6, ptr[rsp]);
			add(rsp, 0x10);
			pop(rdi);
			pop(rdx);
			pop(rcx);
			pop(rbx);

			test(rax, rax);
			je("Return");

			movss(xmm4, ptr[rax + 0x4]);
			movss(xmm5, ptr[rax + 0x8]);
			movaps(ptr[rsp + 0x50], xmm6);
			movaps(xmm1, xmm5);
			movaps(xmm2, xmm4);
			movss(xmm6, ptr[rax]);

			jmp(ptr[rip + retnLabel]);

			L("Return");
			jmp(ptr[rip + jmpLabel]);

			L(funcLabel);
			dq(funcAddr);

			L(jmpLabel);
			dq(targetAddr + 0x13);

			L(retnLabel);
			dq(targetAddr + 0x33);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	AiProcess_Code code(codeBuf, (uintptr_t)GetModifiedZoomData, GetZoomData_Target.GetUIntPtr());
	g_localTrampoline.EndAlloc(code.getCurr());

	g_branchTrampoline.Write6Branch(GetZoomData_Target.GetUIntPtr(), (uintptr_t)code.getCode());
}