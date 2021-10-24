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
