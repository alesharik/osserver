global msr_get
global msr_set

section .text
msr_get:
    mov ecx, [esp + 8]
    rdmsr
    mov [esp + 4], eax
    mov [esp], edx
    ret

msr_set:
    mov ecx, [esp + 8]
    mov eax, [esp + 4]
    mov edx, [esp]
    wrmsr
    ret