#pragma once
namespace ScaleformManager {
	void RegisterScaleform(GFxMovieView* view, GFxValue* f4se_root);

	void ClearMenuSelection();

	class MenuInputHandler : public BSInputEventUser {
	public:
		MenuInputHandler() : BSInputEventUser(true) { }
		virtual void OnButtonEvent(ButtonEvent* inputEvent);
		virtual void OnThumbstickEvent(ThumbstickEvent* inputEvent);
	};

	class PNIOPAMenu : public GameMenuBase {
	public:
		PNIOPAMenu();
		virtual ~PNIOPAMenu() {}
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

	class AllowTextInputHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class ToggleFreezeHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class ResetActorHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class SelectNodeHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class GetNodeListHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class GetNodeNameForSearchHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class GetParentNodeNameHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class GetChildNodesHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class GetSavesHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class SaveHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class LoadSaveHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};

	class DeleteSaveHandler : public GFxFunctionHandler {
	public:
		virtual void Invoke(Args* args) override;
	};
}