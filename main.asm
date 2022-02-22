[bits 64]
segment .text
	global _start
_start:
	call main
	mov rdi, rax
	mov rax, 60
	syscall
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
len:
    cmp     BYTE [rdi], 0
    je      .L4
    mov     eax, 1
    sub     eax, edi
.L3:
    lea     r8d, [rax+rdi]
    add     rdi, 1
    cmp     BYTE [rdi], 0
    jne     .L3
    mov     eax, r8d
    ret
.L4:
    xor     r8d, r8d
    mov     eax, r8d
    ret
println:
	mov rbx, rdi
	call len
	mov r9, rax
	mov rax, 1
	mov rdi, 1
	mov rsi, rbx
	mov rdx, r9
	syscall
	mov rax, 1
	mov rdi, 1
	mov rsi, ln
	mov rdx, 1
	syscall
	ret
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

socket:
	mov rax, 41
	syscall
	ret
connect:
	push rbp
	push dword 0x0    ; IP
	push dword 0x2823 ; PORT
	push word 2       ; AF_INET

	mov rax, 42  ; connect
	mov rsi, rsp ; struct
	mov rdx, 16  ; struct size
	syscall
	pop rbp
	ret
printint:
        push    rbp
        mov     rbp, rsp
        add     rsp, -128
        mov     DWORD [rbp-116], edi
        mov     QWORD [rbp-112], 0
        mov     QWORD [rbp-104], 0
        mov     QWORD [rbp-96], 0
        mov     QWORD [rbp-88], 0
        mov     QWORD [rbp-80], 0
        mov     QWORD [rbp-72], 0
        mov     QWORD [rbp-64], 0
        mov     QWORD [rbp-56], 0
        mov     QWORD [rbp-48], 0
        mov     QWORD [rbp-40], 0
        mov     QWORD [rbp-32], 0
        mov     QWORD [rbp-24], 0
        mov     DWORD [rbp-16], 0
        mov     DWORD [rbp-4], 99
.L2:
        mov     edx, DWORD [rbp-116]
        movsx   rax, edx
        imul    rax, rax, 1717986919
        shr     rax, 32
        sar     eax, 2
        mov     esi, edx
        sar     esi, 31
        sub     eax, esi
        mov     ecx, eax
        mov     eax, ecx
        sal     eax, 2
        add     eax, ecx
        add     eax, eax
        mov     ecx, edx
        sub     ecx, eax
        mov     eax, ecx
        lea     ecx, [rax+48]
        mov     eax, DWORD [rbp-4]
        lea     edx, [rax-1]
        mov     DWORD [rbp-4], edx
        mov     edx, ecx
        cdqe
        mov     BYTE [rbp-112+rax], dl
        mov     eax, DWORD [rbp-116]
        movsx   rdx, eax
        imul    rdx, rdx, 1717986919
        shr     rdx, 32
        sar     edx, 2
        sar     eax, 31
        mov     ecx, eax
        mov     eax, edx
        sub     eax, ecx
        mov     DWORD [rbp-116], eax
        cmp     DWORD [rbp-116], 0
        jne     .L2
        mov     eax, 99
        sub     eax, DWORD [rbp-4]
        cdqe
        mov     edx, DWORD [rbp-4]
        add     edx, 1
        lea     rcx, [rbp-112]
        movsx   rdx, edx
        add     rcx, rdx
        mov     rdx, rax
        mov     rsi, rcx
        mov     edi, 1
		mov rax, 1
		syscall
        nop
        leave
        ret
atoi:
        push    rbp
        mov     rbp, rsp
        mov     QWORD [rbp-24], rdi
        mov     DWORD [rbp-4], 0
        jmp     .L2
.L3:
        mov     edx, DWORD [rbp-4]
        mov     eax, edx
        sal     eax, 2
        add     eax, edx
        add     eax, eax
        mov     DWORD [rbp-4], eax
        mov     rax, QWORD [rbp-24]
        lea     rdx, [rax+1]
        mov     QWORD [rbp-24], rdx
        movzx   eax, BYTE [rax]
        movsx   eax, al
        sub     eax, 48
        add     DWORD [rbp-4], eax
.L2:
        mov     rax, QWORD [rbp-24]
        movzx   eax, BYTE [rax]
        test    al, al
        jne     .L3
        mov     eax, DWORD [rbp-4]
        pop     rbp
        ret
cleaninputbuf:
	xor rbx, rbx
	mov rax, inputstr
	cmp qword [rax+rbx], 0
	je .end
	mov qword [rax+rbx], 0
	inc rbx
.end:
	ret
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
main:
	push rbp
	mov rbp, rsp
	sub rsp, 0
	mov rax, V0
	mov rdi, rax
	call println
	mov eax, 0
	jmp .retpoint
.retpoint:
	add rsp, 0
	pop rbp
	ret
segment .data
	V0 db 0x48,0x65,0x6c,0x6c,0x6f,0x2c,0x20,0x77,0x6f,0x72,0x6c,0x64,0x00
	ln db 0x0A
	text_true db "true", 0x00
	text_false db "false", 0x00
segment .bss
	inputstr resb 50
