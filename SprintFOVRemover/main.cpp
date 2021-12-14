// F4SE
#include "common/IDebugLog.h"  // IDebugLog

#include <shlobj.h>	// CSIDL_MYCODUMENTS

// User Defined Header
#include "Global.h"

#define SPRINTING_STATE	0x0100

bool bDisable1stPersonFOV = true;
bool bDisable3rdPersonFOV = true;

bool IsSprinting() {
	if (!*g_player)
		return false;
	return ((*g_player)->actorState.unk08 & SPRINTING_STATE);
}

void Install() {
	struct AiProcess_Code : Xbyak::CodeGenerator {
		AiProcess_Code(void* buf, UInt64 targetAddr, UInt64 funcAddr) : Xbyak::CodeGenerator(4096, buf) {
			Xbyak::Label funcLabel;
			Xbyak::Label retnLabel;

			push(rax);
			push(rcx);
			call(ptr[rip + funcLabel]);
			and (al, 0x01);
			pop(rcx);
			pop(rax);
			jne("Return");

			mov(ptr[rcx + 0x17C], eax);

			L("Return");
			jmp(ptr[rip + retnLabel]);

			L(funcLabel);
			dq(funcAddr);

			L(retnLabel);
			dq(targetAddr + 0x06);
		}
	};

	if (bDisable1stPersonFOV) {
		uintptr_t _1stPersonFOVAdjust_Target = RelocAddr<uintptr_t>(0x1243943);
		void* codeBuf = g_localTrampoline.StartAlloc();
		AiProcess_Code code(codeBuf, _1stPersonFOVAdjust_Target, (uintptr_t)IsSprinting);
		g_localTrampoline.EndAlloc(code.getCurr());
		g_branchTrampoline.Write6Branch(_1stPersonFOVAdjust_Target, (uintptr_t)code.getCode());
	}

	if (bDisable3rdPersonFOV) {
		uintptr_t _3rdPersonFOVAdjust_Target = RelocAddr<uintptr_t>(0x12519AB);
		void* codeBuf = g_localTrampoline.StartAlloc();
		AiProcess_Code code(codeBuf, _3rdPersonFOVAdjust_Target, (uintptr_t)IsSprinting);
		g_localTrampoline.EndAlloc(code.getCurr());
		g_branchTrampoline.Write6Branch(_3rdPersonFOVAdjust_Target, (uintptr_t)code.getCode());
	}
}

/* Plugin Query */
extern "C" {
	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info) {
		std::string logPath{ "\\My Games\\Fallout4\\F4SE\\" PLUGIN_NAME ".log" };
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath.c_str());
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PLUGIN_NAME;
		info->version = PLUGIN_VERSION;

		if (f4se->runtimeVersion != RUNTIME_VERSION_1_10_163) {
			_MESSAGE("unsupported runtime version %d", f4se->runtimeVersion);
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se) {
		_MESSAGE("%s Loaded", PLUGIN_NAME);

		if (!g_branchTrampoline.Create(1024 * 64)) {
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return false;
		}
		if (!g_localTrampoline.Create(1024 * 64, nullptr)) {
			_ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return false;
		}

		GetConfigValue("Settings", "bRemove1stPersonFOV", &bDisable1stPersonFOV);
		GetConfigValue("Settings", "bRemove3rdPersonFOV", &bDisable3rdPersonFOV);
		_MESSAGE("bRemove1stPersonFOV: %d", bDisable1stPersonFOV);
		_MESSAGE("bRemove3rdPersonFOV: %d", bDisable3rdPersonFOV);

		Install();

		return true;
	}
};