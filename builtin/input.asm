input:
	mov rbx, rsi
	call print
	call cleaninputbuf
	mov rax, 0
	mov rdi, 0
	mov rsi, inputstr
	mov rdx, 50
	syscall
	mov rdi, inputstr
	call rminputstrnl
	mov rax, inputstr
	ret

