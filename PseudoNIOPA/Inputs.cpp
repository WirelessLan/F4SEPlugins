#include "Global.h"

std::unordered_set<UInt32> g_pressedButtons;

namespace Inputs {
	PNIOPAInputHandler g_inputHandler;

	void RegisterInputHandler() {
		g_inputHandler.enabled = true;
		PlayerInputHandler* inputHandler = &g_inputHandler;
		SInt64 idx = (*g_playerControls)->inputEvents1.GetItemIndex(inputHandler);
		if (idx == -1)
			(*g_playerControls)->inputEvents1.Insert(0, inputHandler);
	}

	void PNIOPAInputHandler::OnButtonEvent(ButtonEvent* inputEvent)	{
		if (inputEvent->deviceType != InputEvent::kDeviceType_Keyboard)
			return;

		UInt32 keyCode = inputEvent->keyMask;
		UInt32 isDown = inputEvent->isDown == 1.0f;

		if (isDown) {
			if (g_pressedButtons.find(keyCode) == g_pressedButtons.end())
				g_pressedButtons.insert(keyCode);
		}
		else {
			if (g_pressedButtons.find(keyCode) != g_pressedButtons.end())
				g_pressedButtons.erase(keyCode);
		}

		// Increase Translation
		if (keyCode == g_pluginSettings.IncTransKey && isDown) {
			// , Key
			if (g_pressedButtons.find(g_pluginSettings.DirXKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_X, true);
			// . Key
			else if (g_pressedButtons.find(g_pluginSettings.DirYKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_Y, true);
			// / Key
			else if (g_pressedButtons.find(g_pluginSettings.DirZKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_Z, true);
		}
		// Decrease Translation
		else if (keyCode == g_pluginSettings.DecTransKey && isDown) {
			// , Key
			if (g_pressedButtons.find(g_pluginSettings.DirXKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_X, false);
			// . Key
			else if (g_pressedButtons.find(g_pluginSettings.DirYKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_Y, false);
			// / Key
			else if (g_pressedButtons.find(g_pluginSettings.DirZKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_Z, false);
		}
		// Increase Rotation
		else if (keyCode == g_pluginSettings.IncRotKey && isDown) {
			// , Key
			if (g_pressedButtons.find(g_pluginSettings.DirXKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_X, true);
			// . Key
			else if (g_pressedButtons.find(g_pluginSettings.DirYKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_Y, true);
			// / Key
			else if (g_pressedButtons.find(g_pluginSettings.DirZKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_Z, true);
		}
		// Decrease Rotation
		else if (keyCode == g_pluginSettings.DecRotKey && isDown) {
			// , Key
			if (g_pressedButtons.find(g_pluginSettings.DirXKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_X, false);
			// . Key
			else if (g_pressedButtons.find(g_pluginSettings.DirYKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_Y, false);
			// / Key
			else if (g_pressedButtons.find(g_pluginSettings.DirZKey) != g_pressedButtons.end())
				Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_Z, false);
		}
		// Increase Scale
		else if (keyCode == g_pluginSettings.IncScaleKey && isDown) {
			Nodes::ModNode(Nodes::ModType::kModType_Scale, Nodes::ModDirection::kModDirection_X, true);
		}
		// Decrease Scale
		else if (keyCode == g_pluginSettings.DecScaleKey && isDown) {
			Nodes::ModNode(Nodes::ModType::kModType_Scale, Nodes::ModDirection::kModDirection_X, false);
		}
	}
}