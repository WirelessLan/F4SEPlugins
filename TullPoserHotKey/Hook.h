#pragma once
namespace Hook {
	class IdleCollection : BaseFormComponent {
	public:
		UInt64	unk08;				// 08
		UInt64	unk10;				// 10
		UInt64	unk18;				// 18
		UInt64	unk20;				// 20
		UInt64	unk28;				// 28
		TESIdleForm** idleArray;	// 30
		UInt64	allocCnt;			// 38
		UInt64	idleCount;			// 40
	};

	void Hooks_IdleReady();
}