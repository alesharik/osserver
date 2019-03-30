_kernel_start:

KERNEL_VIRTUAL_BASE equ 0xC0000000

section .text
global _start:function (_start.end - _start)
bits 32
_start:
	mov esp, stack_top

    add ebx, KERNEL_VIRTUAL_BASE
    push ebx
    push eax

	extern kernel_premain
	call kernel_premain

	extern kernel_main
	call kernel_main

	cli
.hang:	hlt
	jmp .hang
.end:

_text_start:
global kernel_exit
kernel_exit:
    jmp _text_end

global __kernel_hang
__kernel_hang:
    cli
    jmp _start.hang
_text_end:

section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:

_kernel_end: