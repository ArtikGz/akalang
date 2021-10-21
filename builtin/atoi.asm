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
