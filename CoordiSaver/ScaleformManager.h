#pragma once

namespace ScaleformManager {
	class MenuInputHandler : public BSInputEventUser {
	public:
		MenuInputHandler() : BSInputEventUser(true) { }

		virtual void OnButtonEvent(ButtonEvent* inputEvent);
	};

	class CoordiSaverMenu : public GameMenuBase {
	public:
		CoordiSaverMenu();

		virtual ~CoordiSaverMenu() {}

		static IMenu* CreateMenu();

		static void RegisterMenu();

		static void OpenMenu();

		static void CloseMenu();
	};

	class MenuInitializeHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class GetSlotListHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class LoadSlotHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class AddSlotHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class DeleteSlotHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class MenuCloseHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class AllowTextInputHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};
};