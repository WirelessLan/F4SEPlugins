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
		TullPoserHotKeyMenu(UInt32 _flags);

		virtual ~TullPoserHotKeyMenu() {}

		static IMenu* CreateMenu();

		static void RegisterMenu();

		static void OpenMenu(const std::string& menuType);

		static void CloseMenu();

		static std::string MenuType;
	};

	class InitializeHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class CloseHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class AllowTextInputHandler : public GFxFunctionHandler {
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

	class SetExpressionHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class ResetExpressionHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class SaveExpressionHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class GetExpressionsHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class LoadExpressionHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class DeleteExpressionHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};
}