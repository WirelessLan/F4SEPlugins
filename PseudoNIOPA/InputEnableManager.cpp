#include "Global.h"

namespace InputEnableManager {
	class BSInputEnableManager {
	public:
		enum {
			kUserEvent_Movement = 1 << 0,
			kUserEvent_Looking = 1 << 1,
			kUserEvent_Activate = 1 << 2,
			kUserEvent_Menu = 1 << 3,
			kUserEvent_Console = 1 << 4,
			kUserEvent_POVSwitch = 1 << 5,
			kUserEvent_Fighting = 1 << 6,
			kUserEvent_Sneaking = 1 << 7,
			kUserEvent_MainFourMenu = 1 << 8,
			kUserEvent_WheelZoom = 1 << 9,
			kUserEvent_Jumping = 1 << 10
		};

		enum {
			kOtherEvent_JournalTabs = 1 << 0,
			kOtherEvent_Activation = 1 << 1,
			kOtherEvent_FastTravel = 1 << 2,
			kOtherEvent_POVChange = 1 << 3,
			kOtherEvent_VATS = 1 << 4,
			kOtherEvent_Favorites = 1 << 5,
			kOtherEvent_PipboyLight = 1 << 6,
			kOtherEvent_ZKey = 1 << 7,
			kOtherEvent_Running = 1 << 8
		};

		struct InputEnableLayerState {
			std::uint32_t index;
			std::uint32_t state;
		};

		static BSInputEnableManager* GetSingleton() {
			RelocPtr<BSInputEnableManager*> g_inputEnableManager(0x58D0780);
			if (!*g_inputEnableManager)
				return nullptr;
			return *g_inputEnableManager;
		}

		bool EnableUserEvent(std::uint32_t a_layerIdx, std::uint32_t a_flag, bool a_enable, std::uint32_t arg4) {
			using func_t = bool(*)(BSInputEnableManager*, std::uint32_t, std::uint32_t, bool, std::uint32_t);
			RelocAddr<func_t> func(0x1B21EA0);
			return func(this, a_layerIdx, a_flag, a_enable, arg4);
		}

		bool EnableOtherEvent(std::uint32_t a_layerIdx, std::uint32_t a_flag, bool a_enable, std::uint32_t arg4) {
			using func_t = bool(*)(BSInputEnableManager*, std::uint32_t, std::uint32_t, bool, std::uint32_t);
			RelocAddr<func_t> func(0x1B21FC0);
			return func(this, a_layerIdx, a_flag, a_enable, arg4);
		}

		void ResetInputEnableLayer(std::uint32_t a_layerIdx) {
			std::uint64_t flag = static_cast<std::uint64_t>(-1);
			EnableUserEvent(a_layerIdx, flag & 0xFFFFFFFF, true, 3);
			EnableOtherEvent(a_layerIdx, flag >> 32, true, 3);
		}

		std::uint64_t unk00[0x118 >> 3];				// 000
		std::uint64_t currentInputEnableMask;			// 118
		std::uint64_t unk120;							// 120
		SimpleLock inputEnableArrLock;					// 128
		tArray<std::uint64_t>	inputEnableMaskArr;		// 130
		tArray<InputEnableLayerState*> layerStateArr;	// 148
		tArray<BSFixedString> layerNameArr;				// 160
	};

	bool g_inputEnableLayerEnabled = false;
	std::uint32_t g_inputEnableLayerIndex = 0xFFFFFFFF;

	bool SetInputEnableLayer(std::uint32_t a_userEventFlag, std::uint32_t a_otherEventFlag) {
		BSInputEnableManager* g_inputEnableManager = BSInputEnableManager::GetSingleton();
		if (!g_inputEnableManager)
			return false;

		g_inputEnableManager->inputEnableArrLock.Lock();

		g_inputEnableLayerIndex = 0xFFFFFFFF;
		for (std::uint32_t ii = 0; ii < g_inputEnableManager->layerStateArr.count; ii++) {
			if (g_inputEnableManager->layerStateArr.entries[ii]->state != 1)
				continue;

			g_inputEnableLayerIndex = g_inputEnableManager->layerStateArr.entries[ii]->index;
			break;
		}

		if (g_inputEnableLayerIndex == 0xFFFFFFFF) {
			g_inputEnableManager->inputEnableArrLock.Release();
			return false;
		}

		g_inputEnableManager->layerStateArr.entries[g_inputEnableLayerIndex]->state = 2;
		g_inputEnableManager->layerNameArr[g_inputEnableLayerIndex] = "Pseudo NIOPA Menu Input Layer";
		g_inputEnableManager->EnableUserEvent(g_inputEnableLayerIndex, a_userEventFlag, false, 3);
		g_inputEnableManager->EnableOtherEvent(g_inputEnableLayerIndex, a_otherEventFlag, false, 3);

		g_inputEnableManager->inputEnableArrLock.Release();

		return true;
	}

	void SetInputEnableLayer() {
		if (g_inputEnableLayerEnabled)
			return;

		g_inputEnableLayerEnabled = true;
		std::uint32_t userEventFlag = BSInputEnableManager::kUserEvent_Sneaking | BSInputEnableManager::kUserEvent_MainFourMenu;
		std::uint32_t otherEventFlag = BSInputEnableManager::kOtherEvent_JournalTabs | BSInputEnableManager::kOtherEvent_Activation | BSInputEnableManager::kOtherEvent_POVChange
			| BSInputEnableManager::kOtherEvent_VATS | BSInputEnableManager::kOtherEvent_Favorites | BSInputEnableManager::kOtherEvent_Running;

		SetInputEnableLayer(userEventFlag, otherEventFlag);
	}

	bool ResetInputEnableLayer(std::uint32_t a_layerIdx) {
		if (a_layerIdx == 0xFFFFFFFF)
			return false;

		BSInputEnableManager* g_inputEnableManager = BSInputEnableManager::GetSingleton();
		if (!g_inputEnableManager)
			return false;

		g_inputEnableManager->inputEnableArrLock.Lock();

		std::uint64_t flag = 0xFFFFFFFFFFFFFFFF;
		g_inputEnableManager->EnableUserEvent(a_layerIdx, flag & 0xFFFFFFFF, true, 3);
		g_inputEnableManager->EnableOtherEvent(a_layerIdx, flag >> 32, true, 3);
		g_inputEnableManager->layerStateArr[a_layerIdx]->state = 1;
		g_inputEnableManager->layerNameArr[a_layerIdx] = "";

		g_inputEnableManager->inputEnableArrLock.Release();

		return true;
	}

	void ResetInputEnableLayer() {
		if (!g_inputEnableLayerEnabled)
			return;

		g_inputEnableLayerEnabled = false;
		ResetInputEnableLayer(g_inputEnableLayerIndex);
	}
}