extrn AdjustPosition:proc, SavePosition:proc, cloningWeapon_Target:Near, handleButtonEvent_Target:Near

.code
	Hook_CloningWeapon proc
		mov rcx, [rsp + 040h]
		mov rdx, [rsp + 0B0h]
		call AdjustPosition

		cmp qword ptr [rsp + 030h], 00h
		mov rax, qword ptr [cloningWeapon_Target]
		add rax, 06h
		jmp rax

		ret
	Hook_CloningWeapon endp
	
	Hook_HandleButtonEvent proc
		cmp dword ptr[rsp + 020h], 065h
		jne DEFAULT
		call SavePosition

	DEFAULT:
		mov rax, qword ptr [handleButtonEvent_Target]
		add rax, 05h
		cmp dword ptr[rsp + 020h], 06Bh
		jmp rax

		ret
	Hook_HandleButtonEvent endp
end