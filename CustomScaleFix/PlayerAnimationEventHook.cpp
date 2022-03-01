#include "Global.h"

RelocAddr<uintptr_t> PlayerAnimationEvent_Target(0x2D442D8 + 0x8);
_PlayerAnimationEvent PlayerAnimationEvent_Original;

EventResult PlayerAnimationEvent_Hook(void* arg1, BSAnimationGraphEvent* evn, void* dispatcher) {
	static BSFixedString cameraOverrideStart("CameraOverrideStart");
	static BSFixedString cameraOverrideStop("CameraOverrideStop");

	if (evn->name == cameraOverrideStart)
		ModifyScale(*g_player, true, 1.0f);
	else if (evn->name == cameraOverrideStop)
		ModifyScale(*g_player, true, GetScale(*g_player));

	return PlayerAnimationEvent_Original(arg1, evn, dispatcher);
}

void Hooks_PlayerAnimationEvent() {
	PlayerAnimationEvent_Original = *(_PlayerAnimationEvent *)(PlayerAnimationEvent_Target.GetUIntPtr());
	SafeWrite64(PlayerAnimationEvent_Target, (uintptr_t)PlayerAnimationEvent_Hook);
}