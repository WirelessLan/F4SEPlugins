#include "Global.h"

namespace Utility {
	void Trim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
			}));
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	//typedef void(*_ExecuteCommand)(const char* str);
	//RelocAddr <_ExecuteCommand> ExecuteCommand_Internal(0x125B4A0);

	// AIProcess::PlayIdle(Actor* a, uint32_t flag, TESIdleForm* idle, bool unk01 = true, uint64_t unk02 = 0)
	typedef bool (*_PlayIdle)(Actor::MiddleProcess* aiProc, Actor* actor, uint32_t flag, TESIdleForm* idle, bool unk01, uint64_t unk02);
	RelocAddr <_PlayIdle> PlayIdle_Internal(0x0E35510);

	bool PlayIdle(Actor* actor, TESIdleForm* idle) {
		if (!actor || !actor->middleProcess || !idle)
			return false;

		return PlayIdle_Internal(actor->middleProcess, actor, 0x35, idle, 0, 0);
	}

	void StopIdle(Actor* actor) {
		static TESIdleForm* idleStop = DYNAMIC_CAST(LookupFormByID(0x000E9855), TESForm, TESIdleForm);
		if (idleStop)
			PlayIdle_Internal(actor->middleProcess, actor, 0x35, idleStop, 1, 0);
	}

	Actor* GetCurrentConsoleActor() {
		UInt32 handle = (*g_consoleHandle);
		NiPointer<TESObjectREFR> refr;
		if (handle != 0 && handle != (*g_invalidRefHandle)) {
			LookupREFRByHandle(handle, refr);
			return DYNAMIC_CAST(refr, TESObjectREFR, Actor);
		}

		return nullptr;
	}
}