#include "Global.h"

namespace InputEnable {
	RelocPtr <BSInputEnableManager*> g_inputEnableManager(0x58D0780);

	typedef bool (*_EnableUserEvent)(BSInputEnableManager*, UInt32, UInt32, bool, UInt32);
	RelocAddr <_EnableUserEvent> EnableUserEvent_Internal(0x1B21EA0);

	typedef bool (*_EnableOtherEvent)(BSInputEnableManager*, UInt32, UInt32, bool, UInt32);
	RelocAddr <_EnableOtherEvent> EnableOtherEvent_Internal(0x1B21FC0);

	bool g_inputEnableLayerEnabled;
	UInt32 g_inputEnableLayerIndex;

	bool SetInputEnableLayer(UInt32& layerIdx, UInt32 userEventFlag, UInt32 otherEventFlag) {
		if (!*g_inputEnableManager)
			return false;

		layerIdx = 0xFFFFFFFF;
		for (UInt32 ii = 0; ii < (*g_inputEnableManager)->layerStateArr.count; ii++) {
			if ((*g_inputEnableManager)->layerStateArr.entries[ii]->state == 1) {
				(*g_inputEnableManager)->layerStateArr.entries[ii]->state = 2;
				layerIdx = (*g_inputEnableManager)->layerStateArr.entries[ii]->index;
				break;
			}
		}

		if (layerIdx == 0xFFFFFFFF)
			return false;

		if (!EnableUserEvent_Internal(*g_inputEnableManager, layerIdx, userEventFlag, false, 3))
			return false;

		if (!EnableOtherEvent_Internal(*g_inputEnableManager, layerIdx, otherEventFlag, false, 3))
			return false;

		return true;
	}

	void SetInputEnableLayer() {
		if (!g_inputEnableLayerEnabled) {
			g_inputEnableLayerEnabled = true;
			UInt32 userEventFlag = BSInputEnableManager::kUserEvent_Menu | BSInputEnableManager::kUserEvent_Fighting;
			UInt32 otherEventFlag = BSInputEnableManager::kOtherEvent_POVChange;
			SetInputEnableLayer(g_inputEnableLayerIndex, userEventFlag, otherEventFlag);
		}
	}

	bool ResetInputEnableLayer(UInt32 layerIdx) {
		UInt64 flag = 0xFFFFFFFFFFFFFFFF;
		EnableUserEvent_Internal(*g_inputEnableManager, layerIdx, flag & 0xFFFFFFFF, true, 3);
		EnableOtherEvent_Internal(*g_inputEnableManager, layerIdx, flag >> 32, true, 3);

		(*g_inputEnableManager)->layerStateArr.entries[layerIdx]->state = 1;

		return true;
	}

	void ResetInputEnableLayer() {
		if (g_inputEnableLayerEnabled) {
			g_inputEnableLayerEnabled = false;
			ResetInputEnableLayer(g_inputEnableLayerIndex);
		}
	}
}