%include 'multiboot.asm'
FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE

section .multiboot
align 4
multiboot_header:
	dd MULTIBOOT_HEADER_MAGIC
	dd FLAGS
	dd -(MULTIBOOT_HEADER_MAGIC + FLAGS)

    dd 0x100000
    dd 0x100000 + 12
    dd 0
    dd 0
    dd 0x100000 + 12
    dd 1 ; Text mode
    dd 1920 ; Screen width
    dd 1080 ; Screen height
    dd 32 ; Pixel depth

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_HEAP_BASE equ 0xC0800000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)

section .data
align 0x1000
boot_page_directory:
    ; This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
    ; All bits are clear except the following:
    ; bit 7: PS The kernel page is 4MB.
    ; bit 1: RW The kernel page is read/write.
    ; bit 0: P  The kernel page is present.
    ; This entry must be here -- otherwise the kernel will crash immediately after paging is
    ; enabled because it can't fetch the next instruction! It's ok to unmap this page later.
    dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0                 ; Pages before kernel space.
    ; This page directory entry defines a 4MB page containing the kernel.
    dd 0x00000083
    dd 0x00100083
    dd boot_page_1 - KERNEL_VIRTUAL_BASE + 3
    dd boot_page_2 - KERNEL_VIRTUAL_BASE + 3
    dd boot_page_3 - KERNEL_VIRTUAL_BASE + 3
    dd boot_page_4 - KERNEL_VIRTUAL_BASE + 3
    dd boot_page_5 - KERNEL_VIRTUAL_BASE + 3
    dd boot_page_6 - KERNEL_VIRTUAL_BASE + 3
    dd boot_page_7 - KERNEL_VIRTUAL_BASE + 3
    dd boot_page_8 - KERNEL_VIRTUAL_BASE + 3
    dd dev_page - KERNEL_VIRTUAL_BASE + 3
    times (1024 - KERNEL_PAGE_NUMBER - 11) dd 0  ; Pages after the kernel image.

; These are pages for kmalloc
boot_page_1:
    times 1024 dd 0
boot_page_2:
    times 1024 dd 0
boot_page_3:
    times 1024 dd 0
boot_page_4:
    times 1024 dd 0
boot_page_5:
    times 1024 dd 0
boot_page_6:
    times 1024 dd 0
boot_page_7:
    times 1024 dd 0
boot_page_8:
    times 1024 dd 0
dev_page:
    times 1024 dd 0

section .text
extern _start
bootloader equ (_bootloader - KERNEL_VIRTUAL_BASE)
global _bootloader:function (_bootloader.end - _bootloader)
_bootloader:

    mov ecx, (boot_page_directory - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx

    mov ecx, cr4
    or ecx, 0x00000010
    mov cr4, ecx

    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx

    lea ecx, [_higher_half]
    jmp ecx
.end:

global _higher_half:function (_higher_half.end - _higher_half)
_higher_half:
    mov dword [boot_page_directory], 0
    mov ecx, cr3
    mov cr3, ecx

    lea ecx, [_start]
    jmp ecx
.end:

global __get_kernel_page_start:function (__get_kernel_page_start.end - __get_kernel_page_start)
__get_kernel_page_start:
    mov eax, boot_page_1
    ret
.end:

global __get_kernel_page_directory:function (__get_kernel_page_directory.end - __get_kernel_page_directory)
__get_kernel_page_directory:
    mov eax, boot_page_directory
    ret
.end:

global __get_kernel_page_directory_size:function (__get_kernel_page_directory_size.end - __get_kernel_page_directory_size)
__get_kernel_page_directory_size:
    mov eax, KERNEL_PAGE_NUMBER
    ret
.end:

global __tlb_flush:function (__tlb_flush.end - __tlb_flush)
__tlb_flush:
    mov ecx, cr3
    mov cr3, ecx
.end:

global __get_kernel_dev_page:function (__get_kernel_dev_page.end - __get_kernel_dev_page)
__get_kernel_dev_page:
    mov eax, dev_page
    ret
.end