#pragma once

class BSAnimationGraphEvent {
public:
	TESObjectREFR* refr;
	BSFixedString name;
	BSFixedString args;
};

typedef EventResult(*_PlayerAnimationEvent)(void* arg1, BSAnimationGraphEvent* evn, void* dispatcher);

void Hooks_PlayerAnimationEvent();