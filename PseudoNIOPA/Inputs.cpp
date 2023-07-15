#include "Global.h"


namespace Inputs {
	std::unordered_set<std::uint32_t> g_pressedButtons;

	class PNIOPAInputHandler : public PlayerInputHandler {
	public:
		PNIOPAInputHandler() : PlayerInputHandler() {}

		virtual void OnButtonEvent(ButtonEvent* a_inputEvent) override {
			if (a_inputEvent->deviceType != InputEvent::kDeviceType_Keyboard)
				return;

			bool isDown = a_inputEvent->isDown == 1.0f;
			std::uint32_t keyCode = a_inputEvent->keyMask;

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
				if (g_pressedButtons.find(g_pluginSettings.DirXKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_X, true);
				else if (g_pressedButtons.find(g_pluginSettings.DirYKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_Y, true);
				else if (g_pressedButtons.find(g_pluginSettings.DirZKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_Z, true);
			}
			// Decrease Translation
			else if (keyCode == g_pluginSettings.DecTransKey && isDown) {
				if (g_pressedButtons.find(g_pluginSettings.DirXKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_X, false);
				else if (g_pressedButtons.find(g_pluginSettings.DirYKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_Y, false);
				else if (g_pressedButtons.find(g_pluginSettings.DirZKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Transition, Nodes::ModDirection::kModDirection_Z, false);
			}
			// Increase Rotation
			else if (keyCode == g_pluginSettings.IncRotKey && isDown) {
				if (g_pressedButtons.find(g_pluginSettings.DirXKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_X, true);
				else if (g_pressedButtons.find(g_pluginSettings.DirYKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_Y, true);
				else if (g_pressedButtons.find(g_pluginSettings.DirZKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_Z, true);
			}
			// Decrease Rotation
			else if (keyCode == g_pluginSettings.DecRotKey && isDown) {
				if (g_pressedButtons.find(g_pluginSettings.DirXKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_X, false);
				else if (g_pressedButtons.find(g_pluginSettings.DirYKey) != g_pressedButtons.end())
					Nodes::ModNode(Nodes::ModType::kModType_Rotation, Nodes::ModDirection::kModDirection_Y, false);
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
	};

	PNIOPAInputHandler g_inputHandler;

	void RegisterInputHandler() {
		if (!*g_playerControls)
			return;

		g_inputHandler.enabled = true;

		PlayerInputHandler* inputHandler = &g_inputHandler;
		std::int64_t idx = (*g_playerControls)->inputEvents1.GetItemIndex(inputHandler);
		if (idx != -1)
			return;

		(*g_playerControls)->inputEvents1.Insert(0, inputHandler);
	}

	void ClearInputs() {
		g_pressedButtons.clear();
	}
}