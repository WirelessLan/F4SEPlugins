extrn AdjustPosition:proc, SavePosition:proc, cloningWeapon_Target:near, handleButtonEvent_Target:near, SavePositionKey:near

.code
	Hook_CloningWeapon proc
		mov rcx, [rsp + 048h]
		mov rdx, [rsp + 0B0h]
		call AdjustPosition

		mov rax, qword ptr [cloningWeapon_Target]
		cmp qword ptr [rsp + 038h], 00h
		je JMP_EQUAL

		add rax, 0Ch
		jmp rax

	JMP_EQUAL:
		add rax, 0643h
		jmp rax
	Hook_CloningWeapon endp
	
	Hook_HandleButtonEvent proc
		mov eax, dword ptr [SavePositionKey]
		cmp dword ptr[rsp + 020h], eax
		jne DEFAULT
		call SavePosition

	DEFAULT:
		mov rax, qword ptr [handleButtonEvent_Target]
		add rax, 062C49h
		mov eax, dword ptr [rax]
		cmp dword ptr[rsp + 020h], eax
		jne JMP_NOT_EQUAL

		mov cl, 01h
		mov rax, qword ptr [handleButtonEvent_Target]
		sub rax, 01B17h
		call rax

	JMP_NOT_EQUAL:
		mov rax, qword ptr [handleButtonEvent_Target]
		add rax, 013h
		jmp rax
	Hook_HandleButtonEvent endp
end