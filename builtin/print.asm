print:
	mov rbx, rdi
	call len
	mov r9, rax
	mov rax, 1
	mov rdi, 1
	mov rsi, rbx
	mov rdx, r9
	syscall
	ret
