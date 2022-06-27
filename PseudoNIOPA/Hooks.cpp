#include "Global.h"

namespace Hooks {
	typedef void (*_PipboyHandler_ReceiveEvent)(void*, ButtonEvent*);
	RelocAddr<uintptr_t> PipboyHandler_ReceiveEvent_Target(0x2D8BB58 + 0x40);
	_PipboyHandler_ReceiveEvent PipboyHandler_ReceiveEvent_Original;

	void PipboyHandler_ReceiveEvent_Hook(void* arg1, ButtonEvent* event) {
		if (Utility::IsActorFrozen(*g_player))
			return;

		PipboyHandler_ReceiveEvent_Original(arg1, event);
	}

	void Install_PipboyHandlerHook() {
		PipboyHandler_ReceiveEvent_Original = *(_PipboyHandler_ReceiveEvent*)(PipboyHandler_ReceiveEvent_Target.GetUIntPtr());
		SafeWrite64(PipboyHandler_ReceiveEvent_Target.GetUIntPtr(), (uintptr_t)PipboyHandler_ReceiveEvent_Hook);
	}
}