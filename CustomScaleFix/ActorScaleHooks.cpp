#include "Global.h"

#include "f4se/xbyak/xbyak.h"

#include "f4se_common/BranchTrampoline.h"
#include "f4se_common/Relocation.h"

typedef void (*_ActorScaleChanged)(Actor*, void*);
RelocAddr <_ActorScaleChanged> ActorScaleChanged(0xDCB200);
_ActorScaleChanged ActorScaleChanged_Original;

typedef NiPoint3* (*_GetZoomData)(BGSZoomData*);
RelocAddr <_GetZoomData> GetZoomData_Target(0x12444E6);

void ActorScaleChaged_Hook(Actor* actor, void* arg2) {
	if (actor == *g_player) 
		UpdateBBX(actor, GetScale(*g_player));
	ActorScaleChanged_Original(actor, arg2);
}

NiPoint3* GetZoomData_Hook(TESObjectWEAP::InstanceData* arg1) {
	static NiPoint3 cameraOffset = {};

	if (!arg1->zoomData)
		return nullptr;

	cameraOffset = arg1->zoomData->zoomData.cameraOffset;
	cameraOffset *= GetScale(*g_player);

	return &cameraOffset;
}

void Hooks_ActorScaleChanged() {
	struct AiProcess_Code : Xbyak::CodeGenerator {
		AiProcess_Code(void* buf) : Xbyak::CodeGenerator(4096, buf) {
			Xbyak::Label retnLabel;

			mov(ptr[rsp + 0x10], rbx);
			mov(ptr[rsp + 0x18], r8b);

			jmp(ptr[rip + retnLabel]);

			L(retnLabel);
			dq(ActorScaleChanged.GetUIntPtr() + 0x0A);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	AiProcess_Code code(codeBuf);
	g_localTrampoline.EndAlloc(code.getCurr());

	ActorScaleChanged_Original = (_ActorScaleChanged)codeBuf;

	g_branchTrampoline.Write6Branch(ActorScaleChanged.GetUIntPtr(), (uintptr_t)ActorScaleChaged_Hook);
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
	AiProcess_Code code(codeBuf, (uintptr_t)GetZoomData_Hook, GetZoomData_Target.GetUIntPtr());
	g_localTrampoline.EndAlloc(code.getCurr());

	g_branchTrampoline.Write6Branch(GetZoomData_Target.GetUIntPtr(), (uintptr_t)code.getCode());
}