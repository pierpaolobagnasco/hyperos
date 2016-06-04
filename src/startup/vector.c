#include <stdint.h>

extern unsigned int __stack;
extern unsigned int _boot_start;

typedef void (* const iHandler)(void);

void __attribute__ ((section(".after_vectors"),weak))
NMI_Handler(void) {
	while (1) {
	}
}

void __attribute__ ((section(".after_vectors"),weak))
HardFault_Handler(void) {
	while (1) {
	}
}

void __attribute__ ((section(".after_vectors"),weak))
MemManage_Handler(void) {
	while (1) {
	}
}

void __attribute__ ((section(".after_vectors"),weak))
BusFault_Handler(void) {
	while (1) {
	}
}

void __attribute__ ((section(".after_vectors"),weak))
UsageFault_Handler(void) {
	while (1) {
	}
}

void __attribute__ ((section(".after_vectors"),weak))
SVC_Handler(void) {
	while (1) {
	}
}

void __attribute__ ((section(".after_vectors"),weak))
SysTick_Handler(void) {
	while (1) {
	}
}

__attribute__ ((section(".isr_vector")))
iHandler isr_vector[] = {
		(iHandler) &__stack,
		(iHandler) &_boot_start,	// Reset Handler
		NMI_Handler,
		HardFault_Handler,
	    MemManage_Handler,
	    BusFault_Handler,
	    UsageFault_Handler,
		0,								// Reserved
		0,								// Reserved
		0,								// Reserved
		0,								// Reserved
		SVC_Handler,
		0,
		0,
		0,
		SysTick_Handler,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
};
