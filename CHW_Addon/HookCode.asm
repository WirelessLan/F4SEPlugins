extrn AdjustPosition:proc, SavePosition:proc, cloningWeapon_JmpTarget:near, handleButtonEvent_JmpTarget:near

.code
	Hook_CloningWeapon proc
		mov rcx, [rsp + 040h]
		mov rdx, [rsp + 0B0h]
		call AdjustPosition

		cmp qword ptr [rsp + 030h], 00h
		jmp qword ptr [cloningWeapon_JmpTarget]
	Hook_CloningWeapon endp
	
	Hook_HandleButtonEvent proc
		cmp dword ptr[rsp + 020h], 065h
		jne DEFAULT
		call SavePosition

	DEFAULT:
		cmp dword ptr[rsp + 020h], 06Bh
		jmp qword ptr [handleButtonEvent_JmpTarget]
	Hook_HandleButtonEvent endp
end