#include "PlayerAnimEvents.h"

#include <f4se/GameReferences.h>
#include <f4se_common/SafeWrite.h>

#include "Utils.h"

namespace PlayerAnimEvents {
	class BSAnimationGraphEvent {
	public:
		TESObjectREFR* refr;
		BSFixedString name;
		BSFixedString args;
	};

	using PlayerAnimationGraphEventHandler_t = EventResult(*)(void*, BSAnimationGraphEvent*, void*);
	RelocAddr<std::uintptr_t> PlayerAnimationGraphEventHandler_Target(0x236A4C8 + 0x8);
	PlayerAnimationGraphEventHandler_t PlayerAnimationGraphEventHandler;

	EventResult PlayerAnimationGraphEventHandler_Hook(void* arg1, BSAnimationGraphEvent* a_evn, void* a_dispatcher) {
		static const BSFixedString cameraOverrideStart("CameraOverrideStart");
		static const BSFixedString cameraOverrideStop("CameraOverrideStop");

		if (a_evn->name == cameraOverrideStart)
			Utils::ModifyFirstPersonScale(*g_player, 1.0f);
		else if (a_evn->name == cameraOverrideStop)
			Utils::ModifyFirstPersonScale(*g_player, Utils::GetScale(*g_player));

		return PlayerAnimationGraphEventHandler(arg1, a_evn, a_dispatcher);
	}

	void Hooks_PlayerAnimEvents() {
		PlayerAnimationGraphEventHandler = *(PlayerAnimationGraphEventHandler_t*)(PlayerAnimationGraphEventHandler_Target.GetUIntPtr());
		SafeWrite64(PlayerAnimationGraphEventHandler_Target, (std::uintptr_t)PlayerAnimationGraphEventHandler_Hook);
	}
}