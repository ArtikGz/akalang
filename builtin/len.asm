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
