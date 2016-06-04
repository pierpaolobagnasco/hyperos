.section .boot, "ax"

.global _boot_start

.extern __bss_start
.extern __bss_end
.extern main

.thumb_func
_boot_start:
/* set the stack pointer */
/* done with c & linker */

/* clear BSS */
	movs r0, #0
	ldr r1, =__bss_start
	ldr r2, =__bss_end
	
0:	
	cmp r1, r2
	bge 1f
	str r0, [r1]
	add r1, #4
	b 0b

1:	bl main
	b 1b

.thumb_func

