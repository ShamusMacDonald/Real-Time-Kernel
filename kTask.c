/*
 * kTask.c
 *
 *  Created on: Oct 13, 2018
 *      Author: Shamus MacDonald
 */
#include <stdlib.h>

#include "kTask.h"
#include "tkCalls.h"

#define UART0_QID 0
#define UNBOUND  -1
#define TRUE    1
#define FALSE   0

#define BUF_SZ  10

// process termination function
extern void t_kill(void);

struct TCB *g_running[PRI_LVLS] = { NULL };
unsigned g_priority = 0;

void reg_task(void (*func)(), uint32_t id, uint32_t pri)
{ 
	uint32_t *p_stktop;
	struct StackFrame *p_stkptr;
	struct TCB *p_tcb;
	 
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
	p_tcb = (struct TCB *)malloc(sizeof(struct TCB));
	
	// Set New TCB attributes
	p_tcb->pid = id;
	p_tcb->psp = (uint32_t)p_stkptr;
	p_tcb->mqid = UNBOUND;
	p_tcb->priority = pri;
	p_tcb->p_stktop = p_stktop;
	p_tcb->blocked = FALSE;

	/* Add new tcb to linked list
	 * If first time called (running NULL)
	 * Then new tcb must be linked to itself
	 * To initialize the linked list
	 */

	// Set global priority if new proc pri highest
    if(pri > g_priority) {
        g_priority = pri;
    }

	if(g_running[pri] == NULL) {

        g_running[pri] = p_tcb;
        g_running[pri]->next = g_running[pri];
        g_running[pri]->prev = g_running[pri];

    }
    else {

        // Add to linked list
        p_tcb->next = g_running[pri]->next;

        p_tcb->prev = g_running[pri];
        p_tcb->next->prev = p_tcb;

        g_running[pri]->next = p_tcb;
        g_running[pri] = p_tcb;

    }

}

unsigned get_psp(void)
{
    /* Returns contents of PSP (current process stack */
    __asm(" mrs     r0, psp");
    __asm(" bx  lr");
    return 0;   /***** Not executed -- shuts compiler up */
                /***** If used, will clobber 'r0' */
}

unsigned get_msp(void)
{
    /* Returns contents of MSP (main stack) */
    __asm(" mrs     r0, msp");
    __asm(" bx  lr");
    return 0;
}

void set_psp(volatile unsigned ProcessStack)
{
    /* set PSP to ProcessStack */
    __asm(" msr psp, r0");
}

void set_msp(volatile unsigned MainStack)
{
    /* Set MSP to MainStack */
    __asm(" msr msp, r0");
}

void set_lr(volatile unsigned lr)
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

unsigned get_sp()
{
    /**** Leading space required -- for label ****/
    __asm("     mov     r0,SP");
    __asm(" bx  lr");
    return 0;
}

