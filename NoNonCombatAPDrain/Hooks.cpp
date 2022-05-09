#include "Global.h"

namespace Hook {
	// ActorValueOwner vtable + 0x30
	RelocAddr<uintptr_t> ModAV_Target(0x02D44408 + 0x30);
	_ModAV ModAV_Original;

	void ModAV_Hook(ActorValueOwner* owner, ActorValue::ActorValueModifier modifier, ActorValueInfo* avInfo, float value) {
		if (!*g_player || !g_actorValue || owner != &(*g_player)->actorValueOwner || avInfo != g_actorValue->actionPoints) {
			ModAV_Original(owner, modifier, avInfo, value);
			return;
		}

		bool drainAp = true;

		// 전투중이 아님 && AP가 감소함
		if (!(*g_player)->IsInCombat() && modifier == ActorValue::kActorValueModifier_Damage && value < 0.0f)
			drainAp = false;
				
		if (drainAp)
			ModAV_Original(owner, modifier, avInfo, value);
		else
			ModAV_Original(owner, modifier, avInfo, 0.0f);
	}

	void Install_Hooks() {
		ModAV_Original = *(_ModAV*)(ModAV_Target.GetUIntPtr());
		SafeWrite64(ModAV_Target, (uintptr_t)ModAV_Hook);
	}
}