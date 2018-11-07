/*
 * process.c
 *
 *  Created on: Oct 13, 2018
 *      Author: Shamus MacDonald
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "process.h"
#include "KernelCalls.h"
//#include "MessagePassing.h"
#define UART0_RX 0
#define UNBOUND  -1

// process termination function
extern void t_kill(void);

struct PCB *g_running[PRI_LVLS] = { NULL };
uint32_t g_priority = 0;

void reg_proc(void (*func)(), uint32_t id, uint32_t pri)
{ 
	uint32_t *p_stktop;
	struct StackFrame *p_stkptr;
	struct PCB *p_pcb;
	 
	// Get 32bit wide stack of size STACK_SIZE	
	p_stktop = (uint32_t *)malloc(STACK_SIZE * sizeof(uint32_t));

	// Set the stack pointer to the bottom of the stack plus the initial stack frame
	p_stkptr = (struct StackFrame *)( p_stktop + (STACK_SIZE - sizeof(struct StackFrame)) );
	
	// Set Initial values of stack frame
	p_stkptr->r0 = 0x00000000;
	p_stkptr->pc = (uint32_t)func;
	p_stkptr->psr = THUMB_MODE;
	p_stkptr->lr = (uint32_t)t_kill;

	// Create a new Process Control Block
	p_pcb = (struct PCB *)malloc(sizeof(struct PCB));
	
	// Set New PCB attributes
	p_pcb->pid = id;
	p_pcb->psp = (uint32_t)p_stkptr;
	p_pcb->mqid = UNBOUND;
	p_pcb->priority = pri;
	p_pcb->p_stktop = p_stktop;

	/* Add new pcb to linked list
	 * If first time called (running NULL)
	 * Then new pcb must be linked to itself
	 * To initialize the linked list
	 */

	// Set global priority if new proc pri highest
    if(pri > g_priority) {
        g_priority = pri;
    }

	if(g_running[pri] == NULL) {

        g_running[pri] = p_pcb;
        g_running[pri]->next = g_running[pri];
        g_running[pri]->prev = g_running[pri];

    }
    else {

        // Add to linked list
        p_pcb->next = g_running[pri]->next;

        p_pcb->prev = g_running[pri];
        p_pcb->next->prev = p_pcb;

        g_running[pri]->next = p_pcb;
        g_running[pri] = p_pcb;

    }

}

uint32_t get_psp(void)
{
    /* Returns contents of PSP (current process stack */
    __asm(" mrs     r0, psp");
    __asm(" bx  lr");
    return 0;   /***** Not executed -- shuts compiler up */
                /***** If used, will clobber 'r0' */
}

uint32_t get_msp(void)
{
    /* Returns contents of MSP (main stack) */
    __asm(" mrs     r0, msp");
    __asm(" bx  lr");
    return 0;
}

void set_psp(volatile uint32_t ProcessStack)
{
    /* set PSP to ProcessStack */
    __asm(" msr psp, r0");
}

void set_msp(volatile uint32_t MainStack)
{
    /* Set MSP to MainStack */
    __asm(" msr msp, r0");
}

void set_lr(volatile uint32_t lr)
{
    /* Set Link Reg as process return addr
     * Should be a func to kill the proc
     */
    __asm(" mov lr,r0");
}

void volatile save_registers()
{
    /* Save r4..r11 on process stack */
    __asm(" mrs     r0,psp");
    /* Store multiple, decrement before; '!' - update R0 after each store */
    __asm(" stmdb   r0!,{r4-r11}");
    __asm(" msr psp,r0");
}

void volatile restore_registers()
{
    /* Restore r4..r11 from stack to CPU */
    __asm(" mrs r0,psp");
    /* Load multiple, increment after; '!' - update R0 */
    __asm(" ldmia   r0!,{r4-r11}");
    __asm(" msr psp,r0");
}

uint32_t get_sp()
{
    /**** Leading space required -- for label ****/
    __asm("     mov     r0,SP");
    __asm(" bx  lr");
    return 0;
}

// PrintString calls PrintChar for each char in the string
void print_str(char *str)
{
    int i = 0;
    while( i < strlen(str) ) {
        print_ch(str[i]);
        i++;
    }
}

/* PrintChar sends a message to the UART with the data  *
 * passed to the function.                              */
void print_ch(char ch)
{
    t_send(UART0_RX, &ch, sizeof(ch));
}

void print_list(void)
{
    unsigned pri;
    struct PCB *p_pcb;
    char pid_ch;
	
	print_str("\nStart:\n");

	for(pri = 0; pri < PRI_LVLS; pri++) {

	    if(g_running[pri] != NULL) {

            p_pcb = g_running[pri];

            do {

                pid_ch = (char)(p_pcb->pid + '0');

                print_str("\nID: ");
                print_ch(pid_ch);
                print_ch('\n');

                p_pcb = p_pcb->prev;

            } while(p_pcb->pid != g_running[pri]->pid);
        }
	}
}

