#pragma once

namespace Hook {
	typedef void (*_ModAV)(ActorValueOwner*, ActorValue::ActorValueModifier, ActorValueInfo*, float value);
	void Install_Hooks();
}