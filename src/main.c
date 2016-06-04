#include <stdint.h>
#include "system_stm32f4xx.h"

#define REG32(x)			*((volatile uint32_t *)(x))

#define SCB_CCR				0xE000ED14
#define SCB_SHCSR			0xE000ED24
#define MPU_CTRL			0xE000ED94
#define MPU_RNR				0xE000ED98
#define MPU_RBAR			0xE000ED9C
#define MPU_RASR			0xE000EDA0

#define NUM_MPU_REGIONS		3
#define NUM_TASKS			3

typedef struct mpu_regions_struct {
	uint32_t base_addr;
	uint32_t attrs;
} mpu_regions_struct;

//typedef void (*pTask_function)(void);

typedef struct tasks_struct {
	uint32_t stack; // TODO: convertire a indirizzo? (puntatore)
	uint32_t state; // TODO: to define
	void (*pTask_function)(void);
	uint8_t mpu_region;
} tasks_struct;

inline void schedule() __attribute__((always_inline));
void output(const char * const pString);
inline void disable_irqs();
inline void enable_irqs();

inline void set_MSP(uint32_t msp_address) {
	asm volatile("msr MSP, %0"
				 :
				 :"r"(msp_address)
				);
}

inline uint32_t get_MSP() {
	uint32_t msp_address;

	asm volatile("mrs %0, MSP"
				 :"=r"(msp_address)
				 :
				 );

	return msp_address;
}

inline void set_PSP(uint32_t psp_address) {
	asm volatile("msr PSP, %0"
				 :
				 :"r"(psp_address)
				);
}

inline uint32_t get_PSP() {
	uint32_t psp_address;

	asm volatile("mrs %0, PSP"
				 :"=r"(psp_address)
				 :
				 );

	return psp_address;
}

inline void use_PSP() {						// Must be inline! // TODO: not true? (non usa lo stack se non si passano parametri)
	asm volatile("mrs r0, CONTROL");		// Read CONTROL register into r0
	asm volatile("orr r0, r0, 0b010");
	asm volatile("msr CONTROL, r0");		// Set CONTROL register to r0

	asm volatile("isb");					// Instruction Synchronization Barrier:
											// this ensures that instructions after
											// the ISB instruction execute using the
											// new stack pointer.
}

inline void use_MSP() {						// Must be inline!
	asm volatile("mrs r0, CONTROL");		// Read CONTROL register into r0
	asm volatile("and r0, r0, 0b101");
	asm volatile("msr CONTROL, r0");		// Set CONTROL register to r0

	asm volatile("isb");					// Instruction Synchronization Barrier:
											// this ensures that instructions after
											// the ISB instruction execute using the
											// new stack pointer.
}

void set_thread_mode_privileged() {			// TODO: useless? Controlla se si può ritornare in privileged.
	asm volatile("mrs r0, CONTROL");		// Read CONTROL register into r0
	asm volatile("and r0, r0, 0b110");
	asm volatile("msr CONTROL, r0");		// Set CONTROL register to r0

	asm volatile("isb");					// TODO: useless?
}

void set_thread_mode_unprivileged() {
	asm volatile("mrs r0, CONTROL");		// Read CONTROL register into r0
	asm volatile("orr r0, r0, 0b001");
	asm volatile("msr CONTROL, r0");		// Set CONTROL register to r0

	asm volatile("isb");					// TODO: useless?
}

uint8_t get_thread_mode() {					// Returns 0 if privileged; 1 if unprivileged
	uint8_t thread_mode;

	asm volatile("mrs %0, CONTROL"
				 :"=r"(thread_mode)
				 :
				 );

	return thread_mode & 1;					// Only the last bit is needed
}

void enable_svc() {
	REG32(SCB_CCR) |= 0x200;				// Set STKALIGN of CCR to 4-bytes (used to
											// offset arguments in SVC_Handler).

	asm volatile("movs r0, #0");			// Set PRIMASK to 0 to be sure that all
											// exceptions can be activated. TODO: check se è necessario. basta vedere il valore di reset.
	asm volatile("msr PRIMASK, r0");
}

void SVC_Handler() {
	// TODO: implement arguments
	uint32_t svc_code;

	asm volatile("ldr r0, [SP, #24]\n\t"	// The 6-th parameter is the caller's PC
				 "ldrb %0, [r0, #-2]"		// Subtract 2 to get the upper byte of the SVC instruction
				 :"=r"(svc_code)
				 :
				);

	switch (svc_code) {
		case 0:

		break;

		case 1:
			output("miao\n");
		break;

		default:
		break;
	}
}

void output(const char * const pString) {
	asm volatile(
				 "movs r0, #0x04\n\t"		// SYS_WRITE0
				 "ldr r1, %0\n\t"			// Load address of "string"
				 "bkpt #0xAB\n\t"			// Thumb-state semihosting call
				 :
				 :"m"(pString)
				);
}

void disable_irqs() {
	asm volatile("cpsid i");
}

void enable_irqs() {
	asm volatile("cpsie i");
}

// Region ID is the array index
// Initial access permission is NO ACCESS
static const mpu_regions_struct mpu_regions[NUM_MPU_REGIONS] = {
		{
			// Starting from 8-th KByte of Flash;
			// size = 512 KBytes;
			// Ending: 0x0807FFFF
			.base_addr = 0x08000000,
			.attrs = 0b00000000000000100000000000100101
		},
		{
			// Starting from 8-th KByte of SRAM;
			// size = 8 KBytes;
			// Ending: 0x20003FFF
			.base_addr = 0x20002000,
			.attrs = 0b00000000000001100000000000011001
		},
		{
			// Starting from 17-th KByte of SRAM;
			// size = 8 KBytes
			// Ending: 0x20006FFF
			.base_addr = 0x20005000,
			.attrs = 0b00000000000001100000000000011001
		}
};

// TODO: to optimize ALL MPU OPERATIONS (and maybe set as inline)
//		 see -> http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/BIHHHDDJ.html

void initialize_MPU_regions(const mpu_regions_struct * const pMpu_regions) {
	uint8_t i;

	for (i = 0; i < NUM_MPU_REGIONS; ++i) {
		REG32(MPU_RNR) = i;					// Set the memory region referenced
											// by MPU_RBAR and MPU_RASR.

		REG32(MPU_RBAR) =					// We assume that the first 5 bits are zero
				pMpu_regions[i].base_addr;

		REG32(MPU_RASR) = pMpu_regions[i].attrs;
	}
}

void initialize_MPU(const mpu_regions_struct * const pMpu_regions) {
	REG32(SCB_SHCSR) |= 0x10000;			// Enable MemManageFault exception

	initialize_MPU_regions(pMpu_regions);

	asm volatile("isb");
}

void restrict_MPU_region(const uint8_t mem_region) {
	REG32(MPU_RNR) = mem_region;			// Set the memory region to modify
	REG32(MPU_RASR) &= 0xF8FFFFFE;			// Disable MPU region (required? seems so)
											// and set permission bits to 0
	REG32(MPU_RASR) |= 0x01000000;			// Set permissions to privileged only (TODO: can be combined with above?)
	REG32(MPU_RASR) |= 0x1;					// Re-enable MPU region (TODO: can be combined with above?)

	asm volatile("isb");	// TODO: ISB is not required because this function is called during an exception (systick).
							// If so, this function can only be called by an exception!
}

void allow_MPU_region(const uint8_t mem_region) {
	REG32(MPU_RNR) = mem_region;			// Set the memory region to modify
	REG32(MPU_RASR) &= 0xF8FFFFFE;			// Disable MPU region (required? seems so)
											// and set permission bits to 0
	REG32(MPU_RASR) |= 0x03000000;			// Set permissions to FULL ACCESS (TODO: can be combined with above?)
	REG32(MPU_RASR) |= 0x1;					// Re-enable MPU region (TODO: can be combined with above?)

	asm volatile("isb");	// TODO: ISB is not required because this function is called during an exception (systick).
							// If so, this function can only be called by an exception!
}

void idle(void) {
	while(1);
}

void task_function_1(void) {
	//

	while(1) {
		//output("task 1\n");
		*((volatile uint32_t *) 0x20002000) += 1;
	}
}

void task_function_2(void) {
	//

	while(1) {
		//output("task 2\n");
		*((volatile uint32_t *) 0x20005000) += 5;
	}
}

__attribute__ ((naked)) void SysTick_Handler() {
	schedule();
}

tasks_struct tasks[NUM_TASKS];

uint32_t task_idx = 0;

inline void schedule() {
	uint32_t scratch;

	disable_irqs();

	/* Save context */
	asm volatile("mrs %0, PSP\n\t"			// Get the current PSP address
				 "stmdb %0!, {r4-r11}\n\t"	// Store sw registers from PSP
				 "msr PSP, %0\n\t"			// Set the new PSP address
				 :"=r"(scratch)
				);

	tasks[task_idx].stack = get_PSP();		// Save the new PSP address to the task table

	task_idx =
		(task_idx % (NUM_TASKS - 1)) + 1;	// Select the new task

	// TODO: questa parte può essere ottimizzata. E' inutile salvare il PSP se poi bisogna ricambiarlo.
	set_PSP(tasks[task_idx].stack);			// Set the PSP to the address of the new task

	/* Load context */
	asm volatile("mrs %0, PSP\n\t"			// Get the current PSP address
				 "ldmia %0!, {r4-r11}\n\t"	// Load sw registers
				 "msr PSP, %0\n\t"			// Set the new PSP address
				 "cpsie i\n\t"				// Re-enable interrupts
				 "isb\n\t"
				 "bx lr\n\t"
				 :"=r"(scratch)
				);
}

uint32_t stack_idle[16];

void init_task(const uint8_t task_idx, const uint32_t entry) {
	uint32_t * sp = (uint32_t *) tasks[task_idx].stack;

	sp = sp - 16;

//	sp[8] = 0;								// r0
//	sp[9] = 0;								// r1
//	sp[10] = 0;								// r2
//	sp[11] = 0;								// r3
/*	sp[12];									// r12 */
	sp[13] = (uint32_t) &idle;				// lr
	sp[14] = entry;							// pc
	sp[15] = 0x01000000;					// psr

	//if (task_idx != 0) {
		tasks[task_idx].stack = (uint32_t) sp;
	//}
}

void main(void) {
	//SystemInit(); // TODO: da eliminare? E DA RIFARE! I VALORI SONO SBALLATI!!!

	disable_irqs();

	/* Tasks configuration */
	tasks[0].stack = ((uint32_t) &stack_idle) + (16 * 4); // TODO: can be 16?
	tasks[0].state = 0;
	tasks[0].pTask_function = &idle;
	tasks[0].mpu_region = 1;

	tasks[1].stack = (0x20004000);
	tasks[1].state = 0;
	tasks[1].pTask_function = &task_function_1;
	tasks[1].mpu_region = 1;

	tasks[2].stack = 0x20007000;
	tasks[2].state = 0;
	tasks[2].pTask_function = &task_function_2;
	tasks[2].mpu_region = 2;

	/* Tasks init */
	init_task(1, (uint32_t)&task_function_1);
	init_task(2, (uint32_t)&task_function_2);

//	asm volatile(
//				 "ldr r2, =scratchpad \n\t"
//			 	 "add r2, #17*4 \n\t"
//			 	 "msr psp, r2 \n\t"
//			 	 "isb \n\t"
//			 	 "mov r3, #2 \n\t"
//			 	 "msr control, r3 \n\t"		// Use PSP
//			 	 //"bl schedule \n\t"
//				 :
//				 :"r"(scratchpad)
//				);

	set_PSP(tasks[0].stack);
	use_PSP();

	enable_irqs();

	/* Configure SysTick */
	REG32(0xE000E014) = 180000000;
	REG32(0xE000E010) = 0b111;
	REG32(0xE000E018) = 1; // launches schedule()

	//enable_svc();

	//initialize_MPU(mpu_regions);
	//allow_MPU_region(0); // Flash

//	use_PSP();
//	set_thread_mode_unprivileged();

	//(*(tasks[task_idx].pTask_function))();

	while (1);
}
