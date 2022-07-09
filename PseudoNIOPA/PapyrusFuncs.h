#pragma once

namespace PapyrusFuncs {
	struct MESSAGEBOX_BUTTON {
	public:
		BSFixedString	text;			// 00
		UInt64			conditions;		// 08
	};

	template<class T>
	class SimpleList {
	public:
		struct _Node {
		public:
			T* item = nullptr;
			_Node* next = nullptr;
		};

		void Insert(T* item) {
			if (IsEmpty()) {
				root.item = item;
				return;
			}

			_Node* newNode = (_Node*)Heap_Allocate(sizeof(_Node));
			if (!newNode)
				return;

			_Node* ptr = &root;
			while (ptr->next != nullptr)
				ptr = ptr->next;

			newNode->item = item;
			newNode->next = nullptr;

			ptr->next = newNode;
		}

		T* Pop() {
			if (IsEmpty())
				return nullptr;

			_Node* prev = nullptr;
			_Node* ptr = &root;
			while (ptr->next != nullptr) {
				prev = ptr;
				ptr = ptr->next;
			}

			T* itemAddr = ptr->item;

			if (ptr == &root) {
				root.item = nullptr;
				return itemAddr;
			}

			Heap_Free(ptr);
			prev->next = nullptr;
			return itemAddr;
		}

		_Node root;
	private:
		bool IsEmpty() {
			if (root.next != nullptr)
				return false;

			if (root.item != nullptr)
				return false;

			return true;
		}
	};

	class BGSMessage : public TESForm {
	public:
		enum { kTypeID = kFormType_MESG };

		TESFullName						fullName;		// 020
		TESDescription					keywordForm;	// 030
		UInt64							icon;			// 048
		TESQuest*						ownerQuest;		// 50
		SimpleList<MESSAGEBOX_BUTTON>	buttonList;		// 58
		UInt64							swfFile;		// 68
		UInt64							shortName;		// 70
		UInt32							flags;			// 78
		UInt32							displayTime;	// 7C
	};

	void Register(VirtualMachine* vm);
};