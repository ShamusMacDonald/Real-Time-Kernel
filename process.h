/*
 * process.h
 *
 *  Created on: Oct 13, 2018
 *      Author: Shamus MacDonald
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <stdint.h>

#define PRIVATE static

#define STACK_SIZE		1024
#define THUMB_MODE		0x01000000

/* Maximum task priority levels
 * This includes 5 user task priority levels
 * and 1 priority level for the idle task
 */
#define PRI_LVLS       6

#define SVC()       __asm(" SVC #0")
#define disable()   __asm(" cpsid i")
#define enable()    __asm(" cpsie i")

#define MSP_RETURN  0xFFFFFFF9    // LR value: exception return using MSP as SP
#define PSP_RETURN  0xFFFFFFFD    // LR value: exception return using PSP as SP

void set_lr(volatile uint32_t);

uint32_t get_psp(void);
void set_psp(volatile uint32_t);
uint32_t get_msp(void);
void set_msp(volatile uint32_t);
uint32_t get_sp(void);

void volatile save_registers(void);
void volatile restore_registers(void);

// Cortex default stack frame
struct StackFrame
{
	/* Registers saved by s/w (explicit)
	 * There is no actual need to reserve space for R4-R11, other than
	 * for initialization purposes.  Note that r0 is the h/w top-of-stack.
	 */
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	// Stacked by hardware (implicit)
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
};

struct PCB {

	uint32_t psp;
	uint32_t pid;
	uint32_t priority;
	uint32_t mqid;

	/* Top of the entire process stack
	 * Used for freeing the process stack
	 * when the process is killed
	 */
	uint32_t *p_stktop;

	struct PCB *next;
	struct PCB *prev;
}; 

void reg_proc(void (*)(),uint32_t,uint32_t);
void print_str(char *);
void print_ch(char);
void print_list(void);

#endif /* PROCESS_H_ */
