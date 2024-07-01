#pragma once

#include <f4se/GameTypes.h>

#include "BSResource.h"

namespace BSTextureIndex {
	class Index {
		std::uint64_t	unk00;
		void* unk08;
		std::uint32_t	unk10;
		std::uint32_t	unk14;
		std::uint64_t	unk18;
		tArray<BSResource::Stream*>	unk20;
		BSResource::ID	unk38[256];
	};

	void Hooks_AddDataFile();
	void Overwrites_Instruction();
}