rminputstrnl:
	xor rbx, rbx
.INIT:
	cmp [inputstr + rbx], byte 0
	je .END 
	add rbx, 1
	jmp .INIT

.END:
	sub rbx, 1
	mov [inputstr + rbx], byte 0
	ret
