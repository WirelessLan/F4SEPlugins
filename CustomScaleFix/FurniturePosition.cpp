#include "Global.h"

RelocAddr <uintptr_t> GetFurniturePosition_Target(0xE857C2);
RelocAddr <uintptr_t> GetFurniturePosition_Jmp_Target1(0xE8580E);
RelocAddr <uintptr_t> GetFurniturePosition_Jmp_Target2(0xE85836);

std::string furnConfigPath{ "Data\\F4SE\\Plugins\\"  PLUGIN_NAME  "_Furnitures.cfg" };
time_t furnConfigLoadedTime = 0;
std::vector<FurnitureKeywordOffset> furnitureOffsets;

FurnitureKeywordOffset* GetFurnitureKeywordOffset(TESObjectREFR* furn) {
	for (UInt32 ii = 0; ii < furnitureOffsets.size(); ii++) {
		if (HasKeyword(furn->baseForm, furnitureOffsets[ii].keyword))
			return &furnitureOffsets[ii];
	}

	return nullptr;
}

float GetCurrentFurnitureMarkerRotation(TESObjectREFR* actorRefr, TESObjectREFR* furnRefr) {
	Actor* actor = DYNAMIC_CAST(actorRefr, TESObjectREFR, Actor);
	TESFurniture* furnForm = DYNAMIC_CAST(furnRefr->baseForm, TESForm, TESFurniture);
	if (!actor || !furnForm)
		return 0.0f;

	if (!actor->middleProcess || !actor->middleProcess->unk08)
		return 0.0f;

	UInt32 currentMarkerId = reinterpret_cast<UInt32&>(actor->middleProcess->unk08->unk3B0[25]);

	if (furnForm->arr188.count == 0 || furnForm->arr188.count < currentMarkerId)
		return 0.0f;

	return reinterpret_cast<float&>(furnForm->arr188.entries[currentMarkerId].unk00[3]);
}

NiPoint3* GetModifiedFurniturePosition(TESObjectREFR* refr, TESObjectREFR* furniture) {
	static NiPoint3 furniturePos = {};

	if (!refr || !furniture)
		return nullptr;

	if (g_pluginOptions.playerOnlyCustomFurniturePosition) {
		if (refr != *g_player)
			return nullptr;
	}

	FurnitureKeywordOffset* offset = GetFurnitureKeywordOffset(furniture);
	if (!offset)
		return nullptr;

	float horOffset = 0.0f;
	float verOffset = 0.0f;

	if (g_pluginOptions.furniturePositionType == FurniturePositionType::kFurniturePositionType_Relative) {
		float scale = GetFurnitureScale(refr);
		if (scale == 0.0f || scale == 1.0f)
			return nullptr;

		horOffset = offset->horizontal * (1.0f - scale);
		verOffset = offset->vertical * (1.0f - scale);
	}
	else if (g_pluginOptions.furniturePositionType == FurniturePositionType::kFurniturePositionType_Absolute) {
		horOffset = offset->horizontal;
		verOffset = offset->vertical;
	}
	else {
		return nullptr;
	}

	furniturePos = furniture->pos;
	float furnitureRot = furniture->rot.z + GetCurrentFurnitureMarkerRotation(refr, furniture);

	furniturePos.x += horOffset * std::sin(furnitureRot);
	furniturePos.y += horOffset * std::cos(furnitureRot);
	furniturePos.z += verOffset;

	return &furniturePos;
}

void Hooks_GetFurniturePosition() {
	static float addss = 20.0;
	static UInt64 addssaddr = reinterpret_cast<UInt64>(&addss);
	struct AiProcess_Code : Xbyak::CodeGenerator {
		AiProcess_Code(void* buf, UInt64 funcAddr, UInt64 targetAddr) : Xbyak::CodeGenerator(4096, buf) {
			Xbyak::Label funcLabel;
			Xbyak::Label jmpLabel;

			push(rbx);
			push(rsi);
			push(rdi);
			push(rbp);
			push(r12);
			push(r15);
			sub(rsp, 0x20);

			mov(rcx, rbx);
			mov(rdx, r14);
			call(ptr[rip + funcLabel]);

			add(rsp, 0x20);
			pop(r15);
			pop(r12);
			pop(rbp);
			pop(rdi);
			pop(rsi);
			pop(rbx);

			test(rax, rax);
			je("DEFAULT");

			movss(xmm0, ptr[rax]);
			movss(xmm1, ptr[rax + 0x04]);
			movss(ptr[rbp - 0x70], xmm0);
			movss(ptr[rbp - 0x6C], xmm1);
			movss(xmm0, ptr[rax + 0x08]);
			movss(ptr[rbp - 0x68], xmm0);
			jmp(ptr[rip + jmpLabel]);

			L("DEFAULT");
			movss(xmm0, ptr[r14 + 0xD0]);
			movss(xmm1, ptr[r14 + 0xD4]);
			movss(ptr[rbp - 0x70], xmm0);
			movss(ptr[rbp - 0x6C], xmm1);
			movss(xmm0, ptr[r14 + 0xD8]);
			movss(ptr[rbp - 0x68], xmm0);
			jmp(ptr[rip + jmpLabel]);

			L(funcLabel);
			dq(funcAddr);

			L(jmpLabel);
			dq(targetAddr + 0x12);
		}
	};
	void* codeBuf = g_localTrampoline.StartAlloc();
	AiProcess_Code code(codeBuf, (uintptr_t)GetModifiedFurniturePosition, GetFurniturePosition_Target.GetUIntPtr());
	g_localTrampoline.EndAlloc(code.getCurr());

	g_branchTrampoline.Write6Branch(GetFurniturePosition_Target, (uintptr_t)code.getCode());

	unsigned char buf[2] = { 0xEB, 0x11 };
	SafeWriteBuf(GetFurniturePosition_Jmp_Target1, buf, 2);
	buf[1] = 0x03;
	SafeWriteBuf(GetFurniturePosition_Jmp_Target2, buf, 2);
}

bool ShouldLoadFurnitureConfig() {
	struct _stat64 stat;
	if (_stat64(furnConfigPath.c_str(), &stat) != 0)
		return false;

	if (furnConfigLoadedTime == 0 || furnConfigLoadedTime != stat.st_mtime) {
		furnConfigLoadedTime = stat.st_mtime;
		return true;
	}

	return false;
}

void LoadFurnitureConfig() {
	FurnitureConfigReader reader(furnConfigPath);
	furnitureOffsets = reader.ReadConfig();
}