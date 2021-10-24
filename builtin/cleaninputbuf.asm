cleaninputbuf:
	xor rbx, rbx
	mov rax, inputstr
	cmp qword [rax+rbx], 0
	je .end
	mov qword [rax+rbx], 0
	inc rbx
.end:
	ret
