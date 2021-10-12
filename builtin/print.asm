__BUILTIN_PRINT:
	mov r8, rdi
	mov r9, rsi
	mov rax, 1
	mov rdi, 1
	mov rsi, r8
	mov rdx, r9
	syscall
	ret
