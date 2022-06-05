extrn AdjustPosition:proc, SavePosition:proc, cloningWeapon_Target:near, handleButtonEvent_Target:near

.code
	Hook_CloningWeapon proc
		mov rcx, [rsp + 040h]
		mov rdx, [rsp + 0B0h]
		call AdjustPosition

		mov rax, qword ptr [cloningWeapon_Target]
		cmp qword ptr [rsp + 030h], 00h
		je JMP_EQUAL

		add rax, 0Ch
		jmp rax

	JMP_EQUAL:
		add rax, 0643h
		jmp rax
	Hook_CloningWeapon endp
	
	Hook_HandleButtonEvent proc
		cmp dword ptr[rsp + 020h], 065h
		jne DEFAULT
		call SavePosition

	DEFAULT:
		cmp dword ptr[rsp + 020h], 06Bh
		jne JMP_NOT_EQUAL

		mov cl, 01h
		mov rax, qword ptr [handleButtonEvent_Target]
		sub rax, 01DE2h
		call rax

	JMP_NOT_EQUAL:
		mov rax, qword ptr [handleButtonEvent_Target]
		add rax, 0Eh
		jmp rax
	Hook_HandleButtonEvent endp
end