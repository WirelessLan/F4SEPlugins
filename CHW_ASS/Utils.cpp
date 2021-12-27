#include "Globals.h"

NiNode* CreateNiNode(UInt16 children) {
	NiNode* node = (NiNode*)CALL_MEMBER_FN(g_mainHeap, Allocate)(sizeof(NiNode), 0x10, true);
	CALL_MEMBER_FN(node, ctor)(children);
	return node;
}