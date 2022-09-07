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

	char GetNextChar(const std::string& line, UInt32& index) {
		if (index < line.length())
			return line[index++];

		return -1;
	}

	std::string GetNextData(const std::string& line, UInt32& index, char delimeter) {
		char ch;
		std::string retVal = "";

		while ((ch = GetNextChar(line, index)) > 0) {
			if (ch == '#') {
				if (index > 0) index--;
				break;
			}

			if (delimeter != 0 && ch == delimeter)
				break;

			retVal += ch;
		}

		Trim(retVal);
		return retVal;
	}

	typedef bool (*_PlayIdle)(Actor::MiddleProcess* aiProc, Actor* actor, uint32_t flag, TESIdleForm* idle, bool unk01, uint64_t unk02);
	RelocAddr <_PlayIdle> PlayIdle_Internal(0x0E35510);

	TESIdleForm* GetIdleForm(const std::string& pluginName, const std::string& poseName) {
		auto plugin_it = g_idleAnimMap.find(pluginName);
		if (plugin_it == g_idleAnimMap.end())
			return nullptr;

		auto pose_it = plugin_it->second.find(poseName);
		if (pose_it == plugin_it->second.end())
			return nullptr;

		return pose_it->second;
	}

	bool PlayIdle(Actor* actor, TESIdleForm* idle) {
		if (!actor || !actor->middleProcess || !idle)
			return false;

		return PlayIdle_Internal(actor->middleProcess, actor, 0x35, idle, 0, 0);
	}

	void StopIdle(Actor* actor) {
		static TESIdleForm* idleStop = DYNAMIC_CAST(LookupFormByID(0x000E9855), TESForm, TESIdleForm);
		if (idleStop)
			PlayIdle_Internal(actor->middleProcess, actor, 0x35, idleStop, 0, 0);
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