// F4SE
#include "common/IDebugLog.h"  // IDebugLog

#include <shlobj.h>	// CSIDL_MYCODUMENTS

// User Defined Header
#include "Global.h"

void Install() {
	struct AiProcess_Code : Xbyak::CodeGenerator {
		AiProcess_Code(void* buf, UInt64 targetAddr) : Xbyak::CodeGenerator(4096, buf) {
			Xbyak::Label jmpLabel;
			Xbyak::Label retnLabel;

			cmp(r8, 0x4);
			jne("RETURN");

			cmp(byte[rax + 0x10], 0x4F);
			jne("RETURN");

			xorps(xmm0, xmm0);
			movaps(ptr[rcx], xmm0);

			jmp(ptr[rip + jmpLabel]);

			L("RETURN");
			andps(xmm3, xmm4);
			andnps(xmm4, ptr[rcx]);
			jmp(ptr[rip + retnLabel]);

			L(jmpLabel);
			dq(targetAddr + 0x1B);

			L(retnLabel);
			dq(targetAddr + 0x06);
		}
	};

	uintptr_t hookTarget = RelocAddr<uintptr_t>(0x1EC5E60 + 0x19D);
	void* codeBuf = g_localTrampoline.StartAlloc();
	AiProcess_Code code(codeBuf, hookTarget);
	g_localTrampoline.EndAlloc(code.getCurr());
	g_branchTrampoline.Write6Branch(hookTarget, (uintptr_t)code.getCode());
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

		Install();

		return true;
	}
};