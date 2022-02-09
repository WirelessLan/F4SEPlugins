#include "Global.h"

RelocAddr<uintptr_t> PerformInputProcessing_Target(0x2D84100);
_PerformInputProcessing PerformInputProcessing_Original;

#ifdef _HOOK_FORCEAV
RelocAddr<uintptr_t> ForceActorValue_Target(0x4F2B90 + 0x86);
#endif

void PerformInputProcessing_Hook(void* inputEventReceiver, const InputEvent* a_queueHead) {
	bool shouldModify = false;

	if (IsThirdPerson()) {
		if (IsSprinting())
			shouldModify = true;
		else if (IsSneaking() && !IsWeaponDrawn() && IsMoving())
			shouldModify = true;
	}

	if (shouldModify)
		ModifyAnimMult();
	else
		RestoreAnimMult();

	PerformInputProcessing_Original(inputEventReceiver, a_queueHead);
}

#ifdef _HOOK_FORCEAV
bool ForceActorValue_Hook(TESObjectREFR* refr, ActorValueInfo* avInfo, float forceAV) {
	if (refr != *g_player || avInfo != g_actorValue->speedMult)
		return false;

	if (!g_modifiedSpeedMult.IsModified())
		return false;

	RestoreAnimMult();

	float currentVal = (*g_player)->actorValueOwner.GetValue(g_actorValue->speedMult);
	float modVal = forceAV - currentVal;
	(*g_player)->actorValueOwner.Mod(ActorValue::ACTOR_VALUE_MODIFIER::Temp, g_actorValue->speedMult, modVal);

	ModifyAnimMult();

	return true;
}
#endif

void Install() {
	PerformInputProcessing_Original = *(_PerformInputProcessing*)(PerformInputProcessing_Target.GetUIntPtr());
	SafeWrite64(PerformInputProcessing_Target, (uintptr_t)PerformInputProcessing_Hook);

#ifdef _HOOK_FORCEAV
	struct AiProcess_Code : Xbyak::CodeGenerator {
		AiProcess_Code(void* buf, UInt64 targetAddr, UInt64 funcAddr) : Xbyak::CodeGenerator(4096, buf) {
			Xbyak::Label funcLabel;
			Xbyak::Label retnLabel;
			Xbyak::Label jmpLabel;

			push(rdx);
			push(rbx);

			mov(rcx, rbx);
			movss(xmm2, ptr[rsp + 0x50 + 0x10]);
			call(ptr[rip + funcLabel]);

			pop(rbx);
			pop(rdx);

			test(al, al);
			je("Return");

			jmp(ptr[rip + jmpLabel]);

			L("Return");
			mov(rax, ptr[rbx + 0x58]);
			lea(rcx, ptr[rbx + 0x58]);
			jmp(ptr[rip + retnLabel]);

			L(retnLabel);
			dq(targetAddr + 0x08);

			L(jmpLabel);
			dq(targetAddr + 0x29);

			L(funcLabel);
			dq(funcAddr);
		}
	};

	void* codeBuf = g_localTrampoline.StartAlloc();
	AiProcess_Code code(codeBuf, ForceActorValue_Target, (uintptr_t)ForceActorValue_Hook);
	g_localTrampoline.EndAlloc(code.getCurr());
	g_branchTrampoline.Write6Branch(ForceActorValue_Target, (uintptr_t)code.getCode());
#endif
}