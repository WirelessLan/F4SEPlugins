#pragma once

namespace Scaleform {
	void RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root);

	class MenuInputHandler : public BSInputEventUser {
	public:
		MenuInputHandler() : BSInputEventUser(true) { }

		virtual void OnButtonEvent(ButtonEvent* inputEvent);
		virtual void OnThumbstickEvent(ThumbstickEvent* inputEvent);
	};

	class PositionerMenu : public GameMenuBase {
	public:
		PositionerMenu();

		virtual ~PositionerMenu() {}

		static IMenu* CreateMenu();

		static void RegisterMenu();

		static void OpenMenu(float offX, float offY, float offZ);

		static void UpdateMenu(float offX, float offY, float offZ);

		static void CloseMenu();

		static bool IsMenuOpen();
	};

	class MCMFunctionHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class InitializeHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class CloseHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class SetPositionHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};
}