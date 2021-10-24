printbool:
	cmp dil, 0
	jne .TRUE

.FALSE:
	mov rdi, text_false
	call println
	jmp .END

.TRUE:
	mov rdi, text_true
	call println

.END:
	ret
