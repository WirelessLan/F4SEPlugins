#pragma once
namespace Inputs {
	class PNIOPAInputHandler : public PlayerInputHandler {
	public:
		PNIOPAInputHandler() : PlayerInputHandler() { }
		virtual void OnButtonEvent(ButtonEvent* inputEvent) override;
	};

	void RegisterInputHandler();
};