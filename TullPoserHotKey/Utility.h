#pragma once

class TESIdleForm : public TESForm {
public:
	UInt64 conditions;					// 20
	UInt64 data;						// 28
	UInt64* childIdles;					// 30
	TESIdleForm* parentIdle;			// 38
	TESIdleForm* prevIdle;				// 40
	BSFixedString behaviorGraphName;	// 48
	BSFixedString animEventName;		// 50
	BSFixedString animFileName;			// 58
	const char* formEditorID;			// 60
};

namespace Utility {
	void Trim(std::string& s);
	bool PlayIdle(Actor* actor, TESIdleForm* idle);
	void StopIdle(Actor* actor);
	Actor* GetCurrentConsoleActor();
}