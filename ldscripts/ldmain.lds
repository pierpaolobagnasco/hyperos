OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_boot_start)

MEMORY
{
  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 112K
  CCMRAM (xrw) : ORIGIN = 0x00000000, LENGTH = 0
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
}

__stack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
	.isr_vector :
    {
        . = ALIGN(4);
		KEEP(*(.isr_vector))
		*(.after_vectors)
		. = ALIGN(4);
    } > FLASH
	
	.text :
	{
		. = ALIGN(4);
		*(.boot)
		*(.text .text.*)
		*(.rodata .rodata.*)
		. = ALIGN(4);
	} > FLASH
	
	.data :
	{
		. = ALIGN(4);
		*(.data .data.*)
		. = ALIGN(4);
	} > RAM

	.bss :
	{
		. = ALIGN(16);
		__bss_start = .;
		*(.bss .bss.*)
		*(COMMON)
		__bss_end = .;
		. = ALIGN(16);
	} > RAM
}
