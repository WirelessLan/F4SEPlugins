#pragma once

namespace  BSResource {
	struct ID {
		std::uint32_t file;
		std::uint32_t ext;
		std::uint32_t dir;
	};

	class Stream {
	public:
		virtual void Unk_00(void);
		virtual void Unk_01(void);
		virtual void Unk_02(void);
		virtual void Unk_03(void);
		virtual void Unk_04(void);
		virtual void Unk_05(void);
		virtual void Unk_06(void);
		virtual void Unk_07(void);
		virtual void Unk_08(void);
		virtual void Unk_09(void);
		virtual void Unk_0A(void);
		virtual void Unk_0B(void);
		virtual void Unk_0C(void);
		virtual void Unk_0D(void);
		virtual void Unk_0E(void);
		virtual void Unk_0F(void);
		virtual void Unk_10(void*& a_arg1);
	};

	namespace Archive2 {
		class Index {
		public:
			std::uint64_t	unk00;
			std::uint64_t	unk08;
			std::uint64_t	unk10;
			std::uint64_t*	unk18;
			std::uint64_t	unk20;
			std::uint64_t	unk28;
			Stream*			unk30[256];
			void*			unk830[256];
			ID				unk1030[256];
			std::uint64_t	unk1C30;	// count?
		};

		void Hooks_AddDataFile();
	}

	namespace SDirectory2 {
		void Hooks_Retrieve();
	}
}