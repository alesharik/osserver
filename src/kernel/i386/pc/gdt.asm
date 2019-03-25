global __gdt_set
global __gdt_reload_segments

section .data
gdt_struct:
    dw 0 ; limit
    dd 0 ; offset

section .text
__gdt_set:
    mov eax, [esp + 4]
    mov [gdt_struct + 2], eax
    mov ax, [esp + 8]
    mov [gdt_struct], ax
    lgdt [gdt_struct]
    ret

__gdt_reload_segments:
    jmp 0x08:reload_cs
reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov gs, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    ret
