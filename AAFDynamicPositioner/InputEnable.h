#pragma once

namespace InputEnable {
	struct InputEnableLayerState {
		UInt32	index;
		UInt32	state;
	};

	class BSInputEnableManager {
	public:
		enum {
			kUserEvent_Movement = 1 << 0,
			kUserEvent_Looking = 1 << 1,
			kUserEvent_Activate = 1 << 2,
			kUserEvent_Menu = 1 << 3,
			kUserEvent_Console = 1 << 4,
			kUserEvent_POVChange = 1 << 5,
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
			kOtherEvent_FAVORITES = 1 << 5,
			kOtherEvent_PipboyLight = 1 << 6,
			kOtherEvent_ZKey = 1 << 7,
			kOtherEvent_Running = 1 << 8
		};

		UInt64 unk00[0x118 >> 3];
		UInt64 currentInputEnableMask;	// 118
		UInt64 unk120;
		UInt64 unk128;
		tArray<UInt64>	inputEnableMaskArr;	// 130
		tArray<InputEnableLayerState*> layerStateArr;	// 148
		tArray<void*> unk160;	// 160
	};

	void SetInputEnableLayer();
	void ResetInputEnableLayer();
}