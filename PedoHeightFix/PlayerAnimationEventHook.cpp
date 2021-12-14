#include "Global.h"

#include "f4se_common/SafeWrite.h"

typedef EventResult(* _PlayerAnimationEvent)(void *arg1, BSAnimationGraphEvent* evn, void* dispatcher);
_PlayerAnimationEvent PlayerAnimationEvent_Original;

EventResult PlayerAnimationEvent_Hook(void* arg1, BSAnimationGraphEvent* evn, void* dispatcher) {
	static BSFixedString cameraOverrideStart("CameraOverrideStart");
	static BSFixedString cameraOverrideStop("CameraOverrideStop");

	if (evn->name == cameraOverrideStart) 
		SkeletonManager::ModifyScale(*g_player, true, 1.0f);
	else if (evn->name == cameraOverrideStop) 
		SkeletonManager::ModifyScale(*g_player, true, GetPlayerScale());

	return PlayerAnimationEvent_Original(arg1, evn, dispatcher);
}

void Hooks_PlayerAnimationEvent() {
	uintptr_t original_addr = RelocAddr<uintptr_t>(0x2D442D8 + 0x8);
	PlayerAnimationEvent_Original = *(_PlayerAnimationEvent *)(original_addr);
	SafeWrite64(original_addr, (uintptr_t)PlayerAnimationEvent_Hook);
}