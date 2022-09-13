// F4SE
#include "common/IDebugLog.h"  // IDebugLog

#include <shlobj.h>	// CSIDL_MYCODUMENTS

// User Defined Header
#include "Global.h"

UInt32 additionalJumpCnt;
UInt32 uMaxAdditionalJumpCnt = 1;

PluginHandle			g_pluginHandle;
F4SEMessagingInterface* g_messaging;

struct CharacterStateChangeEvent {
	UInt32	toState;
	UInt32	fromState;
};

class hknpCharacterContext {
public:
	enum {
		kCharacterStateType_OnGround,
		kCharacterStateType_Jumping,
		kCharacterStateType_InAir,
		kCharacterStateType_Climbing,
		kCharacterStateType_Flying,
		kCharacterStateType_Swimming
	};

	UInt64	unk00;						// 00
	UInt64	unk08;						// 08
	UInt64	characterType;				// 10
	UInt64	unk18;						// 18
	UInt32	currentState;				// 20
	UInt32	previousState;				// 24
	UInt32	filterEnable;				// 28
	float maxLinearAcceleration;		// 2C
	float maxLinearSpeed;				// 30
	float gain;							// 34
	UInt64	unk38;						// 38
	UInt64	unk40;						// 40
	UInt64	unk48;						// 48
	UInt64	unk50;						// 50
	UInt64	unk58;						// 58
};

class bhkCharacterController {
public:
	UInt64	unk00[0x2A0 >> 3];			// 000
	hknpCharacterContext	context;	// 2A0
	UInt32 flags;						// 300
	UInt32 wantState;					// 304
	float velocityTime;					// 308
	float rotMod;						// 30C
	float rotModTime;					// 310
	float calculatePitchTimer;			// 314
	float acrobatics;					// 318
	float center;						// 31C
	float waterHeight;					// 320
	float jumpHeight;					// 324
	float fallStartHeight;				// 328
	float fallTime;						// 32C
};

class ActiveEffect;
class JetpackEffect;

typedef EventResult(*_bhkCharacterStateChangeEvent_Handler)(void* arg1, CharacterStateChangeEvent* evn, void* dispatcher);
RelocAddr<uintptr_t> bhkCharacterStateChangeEvent_Handler_Target(0x2D44690 + 0x8);
_bhkCharacterStateChangeEvent_Handler bhkCharacterStateChangeEvent_Handler_Original;

typedef void(*_JumpHandler_HandleEvent)(void* arg1, ButtonEvent* event);
RelocAddr<uintptr_t> JumpHandler_HandleEvent_Target(0x2D49D68 + 0x40);
_JumpHandler_HandleEvent JumpHandler_HandleEvent_Original;

EventResult bhkCharacterStateChangeEvent_Handler_Hook(void* arg1, CharacterStateChangeEvent* evn, void* dispatcher) {
	if (evn->toState == hknpCharacterContext::kCharacterStateType_OnGround || evn->toState == hknpCharacterContext::kCharacterStateType_Swimming)
		additionalJumpCnt = 0;
	return bhkCharacterStateChangeEvent_Handler_Original(arg1, evn, dispatcher);
}

bool IsJetpackEffectActive() {
	if (!*g_player || !(*g_player)->middleProcess || !(*g_player)->middleProcess->unk08)
		return false;

	tArray<ActiveEffect*> activeEffectList = reinterpret_cast<tArray<ActiveEffect*>&>((*g_player)->middleProcess->unk08->unk00[71]);
	for (UInt32 ii = 0; ii < activeEffectList.count; ii++) {
		JetpackEffect* jetpackEff = DYNAMIC_CAST(activeEffectList.entries[ii], ActiveEffect, JetpackEffect);
		if (jetpackEff)
			return true;
	}

	return false;
}

void JumpHandler_HandleEvent_Hook(void* arg1, ButtonEvent* evn) {
	bool isDown = evn->isDown == 1.0f && evn->timer == 0.0f;

	if (isDown && !IsJetpackEffectActive() && *g_player && (*g_player)->middleProcess && (*g_player)->middleProcess->unk08) {
		bhkCharacterController* controller = (bhkCharacterController*)(*g_player)->middleProcess->unk08->unk3B0[6];
		if (controller && controller->context.currentState == hknpCharacterContext::kCharacterStateType_InAir) {
			if (additionalJumpCnt < uMaxAdditionalJumpCnt) {
				additionalJumpCnt++;
				controller->context.currentState = 1;
				controller->fallTime = 0;
			}
		}
	}
	JumpHandler_HandleEvent_Original(arg1, evn);
}

std::string GetConfigOption(const char* section, const char* key) {
	char	result[256] = { 0 };
	std::string configPath{ "Data\\F4SE\\Plugins\\" PLUGIN_NAME ".ini" };

	GetPrivateProfileString(section, key, NULL, result, sizeof(result), configPath.c_str());
	return result;
}

bool GetConfigValue(const char* section, const char* key, std::string* dataOut) {
	std::string	data = GetConfigOption(section, key);
	if (data.empty())
		return false;

	*dataOut = data;
	return true;
}

void ReadConfig() {
	static time_t lastConfigRead = 0;

	std::string configPath{ "Data\\F4SE\\Plugins\\" PLUGIN_NAME ".ini" };
	struct _stat64 stat;
	if (_stat64(configPath.c_str(), &stat) != 0)
		return;

	if (lastConfigRead != 0 && lastConfigRead == stat.st_mtime)
		return;

	lastConfigRead = stat.st_mtime;

	std::string out;
	if (GetConfigValue("Settings", "uMaxAdditionalJumpCnt", &out)) {
		uMaxAdditionalJumpCnt = std::stoul(out);
		_MESSAGE("uMaxAdditionalJumpCnt: %u", uMaxAdditionalJumpCnt);
	}
}

void OnF4SEMessage(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_NewGame:
	case F4SEMessagingInterface::kMessage_PreLoadGame:
		ReadConfig();
		additionalJumpCnt = 0;
		break;
	}
}

/* Plugin Query */
extern "C" {
	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info) {
		std::string logPath{ "\\My Games\\Fallout4\\F4SE\\" PLUGIN_NAME ".log" };
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath.c_str());
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PLUGIN_NAME;
		info->version = PLUGIN_VERSION;

		if (f4se->runtimeVersion != RUNTIME_VERSION_1_10_163) {
			_MESSAGE("unsupported runtime version %d", f4se->runtimeVersion);
			return false;
		}

		g_pluginHandle = f4se->GetPluginHandle();

		// Get the messaging interface
		g_messaging = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging) {
			_FATALERROR("couldn't get messaging interface");
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se) {
		_MESSAGE("%s Loaded", PLUGIN_NAME);

		bhkCharacterStateChangeEvent_Handler_Original = *(_bhkCharacterStateChangeEvent_Handler*)(bhkCharacterStateChangeEvent_Handler_Target.GetUIntPtr());
		SafeWrite64(bhkCharacterStateChangeEvent_Handler_Target, (uintptr_t)bhkCharacterStateChangeEvent_Handler_Hook);

		JumpHandler_HandleEvent_Original = *(_JumpHandler_HandleEvent*)(JumpHandler_HandleEvent_Target.GetUIntPtr());
		SafeWrite64(JumpHandler_HandleEvent_Target.GetUIntPtr(), (uintptr_t)JumpHandler_HandleEvent_Hook);

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);

		return true;
	}
};