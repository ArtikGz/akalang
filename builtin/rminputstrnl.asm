rminputstrnl:
        push    rbp
        mov     rbp, rsp
        mov     QWORD [rbp-8], rdi
        jmp     .L2
.L3:
        mov     rax, QWORD [rbp-8]
        movzx   eax, BYTE [rax]
        cmp     al, 10
        jne     .L2
        mov     rax, QWORD [rbp-8]
        lea     rdx, [rax+1]
        mov     QWORD [rbp-8], rdx
        mov     BYTE [rax], 0
.L2:
        mov     rax, QWORD [rbp-8]
        movzx   eax, BYTE [rax]
        test    al, al
        jne     .L3
        nop
        nop
        pop     rbp
        ret
