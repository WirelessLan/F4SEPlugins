#include "Global.h"

namespace Hook {
	typedef void (*_IdleReady)(void*);
	RelocAddr <_IdleReady> IdleReady_HookTarget(0x5AA3B0);
	_IdleReady IdleReady_Original;

	void IdleReady_Hook(IdleCollection* g_idleCol) {
		for (UInt64 ii = 0; ii < g_idleCol->idleCount; ii++) {
			TESIdleForm* idleForm = g_idleCol->idleArray[ii];
			if (!idleForm)
				break;

			if (!idleForm->unk08)
				continue;

			ModInfo* modInfo = idleForm->unk08->entries[0];
			if (!modInfo)
				continue;

			if (!idleForm->formEditorID)
				continue;

			auto plugin_it = g_idleAnimMap.find(modInfo->name);
			if (plugin_it == g_idleAnimMap.end()) {
				auto in_res = g_idleAnimMap.insert(std::make_pair(modInfo->name, CaseInsensitiveMap<TESIdleForm*>()));
				plugin_it = in_res.first;
			}

			plugin_it->second.insert(std::make_pair(idleForm->formEditorID, idleForm));
		}

		IdleReady_Original(g_idleCol);
	}

	void Hooks_IdleReady() {
		struct IdleReady_Code : Xbyak::CodeGenerator {
			IdleReady_Code(void* buf) : Xbyak::CodeGenerator(4096, buf) {
				Xbyak::Label retnLabel;

				mov(ptr[rsp + 0x10], rbx);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(IdleReady_HookTarget.GetUIntPtr() + 0x05);
			}
		};
		void* codeBuf = g_localTrampoline.StartAlloc();
		IdleReady_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		IdleReady_Original = (_IdleReady)codeBuf;

		g_branchTrampoline.Write5Branch(IdleReady_HookTarget.GetUIntPtr(), (uintptr_t)IdleReady_Hook);
	}
}