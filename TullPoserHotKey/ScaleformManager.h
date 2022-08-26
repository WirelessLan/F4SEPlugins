#pragma once

namespace ScaleformManager {
	void RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root);

	void ClearSelectedVars();

	class MenuInputHandler : public BSInputEventUser {
	public:
		MenuInputHandler() : BSInputEventUser(true) { }

		virtual void OnButtonEvent(ButtonEvent* inputEvent);
		virtual void OnThumbstickEvent(ThumbstickEvent* inputEvent);
	};

	class TullPoserHotKeyMenu : public GameMenuBase {
	public:
		TullPoserHotKeyMenu();

		virtual ~TullPoserHotKeyMenu() {}

		static IMenu* CreateMenu();

		static void RegisterMenu();

		static void OpenMenu();

		static void CloseMenu();
	};

	class InitializeHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class CloseHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class GetPluginListHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class SelectPluginHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class GetPoseListHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class SelectPoseHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};
}