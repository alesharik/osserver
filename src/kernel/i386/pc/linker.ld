ENTRY(_bootloader)

KERNEL_VIRTUAL_BASE = 0xC0000000;
KERNEL_PHYSICAL_BASE = 0x100000;

SECTIONS
{
    . = KERNEL_VIRTUAL_BASE + KERNEL_PHYSICAL_BASE;

    _kernel_start = .;
    .text BLOCK(4K) : AT(ADDR(.text) - KERNEL_VIRTUAL_BASE)
	{
	    *(.multiboot)
		*(.text)
		. = ALIGN(4K);
	}

	.rodata BLOCK(4K) : AT(ADDR(.rodata) - KERNEL_VIRTUAL_BASE)
	{
		*(.rodata)
		. = ALIGN(4K);
	}

	.data BLOCK(4K) : AT(ADDR(.data) - KERNEL_VIRTUAL_BASE)
	{
		*(.data)
		. = ALIGN(4K);
	}

	.bss BLOCK(4K) : AT(ADDR(.bss) - KERNEL_VIRTUAL_BASE)
	{
		*(COMMON)
		*(.bss)
		. = ALIGN(4K);
	}

    _kernel_end = .;
    /DISCARD/ : {
        *(.note.gnu.build-id)
    }
}