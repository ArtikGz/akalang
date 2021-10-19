input:
	mov rbx, rsi
	call print
	mov rax, 0
	mov rdi, 0
	mov rsi, inputstr
	mov rdx, 50
	syscall
	mov rax, inputstr
	ret

