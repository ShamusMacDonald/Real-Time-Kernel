/*
 *  SVC.c
 *
 *  Created on: Oct 16, 2018
 *  Author: Shamus MacDonald
 *  Disclaimer: The declarations contained in this file and the code contained
 *      in the associated c file were edited to be used in this project from work
 *      obtained from Dr. Larry Hughes in ECED4402 - Real Time Systems.
 *  Description: Responsible for handling supervisory calls to the kernel. The
 *      first call is responsible for initializing the first process stack pointer,
 *      subsequent calls are from processes during execution.
 */

#include <stdlib.h>
#include <stdint.h>
#include "process.h"
#include "KernelCalls.h"
#include "messaging.h"

#define TRUE    1
#define FALSE   0
#define PRIVATE static

// Forward declared SVC funcs
void SVCall(void);
void SVCHandler(struct StackFrame *);

// Forward declared kernel funcs
PRIVATE void k_kill(void);

// User defined clock initialization
extern void InitClock(void);

// Currently running process
extern struct PCB *g_running[PRI_LVLS];
extern uint32_t g_priority;

void SVCall(void)
{
    /* Supervisor call (trap) entry point
     * Using MSP - trapping process either MSP or PSP (specified in LR)
     * Source is specified in LR: F9 (MSP) or FD (PSP)
     * Save r4-r11 on trapping process stack (MSP or PSP)
     * Restore r4-r11 from trapping process stack to CPU
     * SVCHandler is called with r0 equal to MSP or PSP to access any arguments
     */

    /* Save LR for return via MSP or PSP */
    __asm("     PUSH    {LR}");

    /* Trapping source: MSP or PSP? */
    __asm("     TST     LR,#4");    /* Bit #4 indicates MSP (0) or PSP (1) */
    __asm("     BNE     RtnViaPSP");

    /* Trapping source is MSP - save r4-r11 on stack (default, so just push) */
    __asm("     PUSH    {r4-r11}");
    __asm("     MRS r0,msp");
    __asm("     BL  SVCHandler");   /* r0 is MSP */
    __asm("     POP {r4-r11}");
    __asm("     POP     {PC}");

    /* Trapping source is PSP - save r4-r11 on psp stack (MSP is active stack) */
    __asm("RtnViaPSP:");
    __asm("     mrs     r0,psp");
    __asm("     stmdb   r0!,{r4-r11}"); /* Store multiple, decrement before */
    __asm("     msr psp,r0");
    __asm("     BL  SVCHandler");   /* r0 Is PSP */

    /* Restore r4..r11 from trapping process stack  */
    __asm("     mrs     r0,psp");
    __asm("     ldmia   r0!,{r4-r11}"); /* Load multiple, increment after */
    __asm("     msr psp,r0");
    __asm("     POP     {PC}");

}

void SVCHandler(struct StackFrame *p_procstk)
{
    /*
     * Supervisor call handler
     * Handle startup of initial process
     * Handle all other SVCs such as getid, terminate, etc.
     * Assumes first call is from startup code
     * Argptr points to (i.e., has the value of) either:
       - the top of the MSP stack (startup initial process)
       - the top of the PSP stack (all subsequent calls)
     * Argptr points to the full stack consisting of both hardware and software
       register pushes (i.e., R0..xPSR and R4..R10); this is defined in type
       stack_frame
     * Argptr is actually R0 -- setup in SVCall(), above.
     * Since this has been called as a trap (Cortex exception), the code is in
       Handler mode and uses the MSP
     */
    static int firstSVCcall = TRUE;
    struct KernelCallArgs *p_kernargs;

    if (firstSVCcall)
    {
    /*
     * Force a return using PSP
     * This will be the first process to run, so the eight "soft pulled" registers
       (R4..R11) must be ignored otherwise PSP will be pointing to the wrong
       location; the PSP should be pointing to the registers R0..xPSR, which will
       be "hard pulled"by the BX LR instruction.
     * To do this, it is necessary to ensure that the PSP points to (i.e., has) the
       address of R0; at this moment, it points to R4.
     * Since there are eight registers (R4..R11) to skip, the value of the sp
       should be increased by 8 * sizeof(unsigned int).
     * sp is increased because the stack runs from low to high memory.
    */
        set_psp(g_running[g_priority]->psp + 8 * sizeof(uint32_t));

        firstSVCcall = FALSE;
        /* Start SysTick */
        InitClock();
        // init msg passing
        init_msg();

        /*
         - Change the current LR to indicate return to Thread mode using the PSP
         - Assembler required to change LR to FFFF.FFFD (Thread/PSP)
         - BX LR loads PC from PSP stack (also, R0 through xPSR) - "hard pull"
        */
        __asm(" movw    LR,#0xFFFD");  /* Lower 16 [and clear top 16] */
        __asm(" movt    LR,#0xFFFF");  /* Upper 16 only */
        __asm(" bx  LR");          /* Force return to PSP */
    }
    else /* Subsequent SVCs */
    {
        /*
         * p_kernargs points to the arguments associated with this kernel call
         * p_procstk is the value of the PSP (passed in R0 and pointing to the TOS)
         * the TOS is the complete stack_frame (R4-R11, R0-xPSR)
         * in this example, R7 contains the address of the structure supplied by
            the process - the structure is assumed to hold the arguments to the
            kernel function.
         * to get the address and store it in p_kernargs, it is simply a matter of
           assigning the value of R7 (arptr -> r7) to kcaptr
         */
        p_kernargs = (struct KernelCallArgs *)p_procstk->r7;

        switch(p_kernargs->code) {

        case GETID:
            p_kernargs->rtnval = g_running[g_priority]->pid;
            break;

        case KILL:
            k_kill();
            break;

        case BIND:
            break;

        case SEND:
            p_kernargs->rtnval = k_send(p_kernargs->arg3               // dst
                                      , g_running[g_priority]->mqid    // src
                                      , (void *)p_kernargs->arg1       // data
                                      , p_kernargs->arg2 );            // size
            break;

        default:
            p_kernargs->rtnval = BADCODE;
        }
    }

}
/* k_kill is the privileged function responsible for
 * killing (terminating) the running process. This requires
 * freeing the tasks allocated stack, adjusting the running
 * queue, and freeing the PCB.
 * Control is then returned to the next process in queue.
 */
void k_kill(void)
{
    struct PCB *temp;

    // free task stack
    free(g_running[g_priority]->p_stktop);

    temp = g_running[g_priority];

    // Is this the last task in the queue?
    if(g_running[g_priority] == g_running[g_priority]->next) {

        // Set this queue as empty
        g_running[g_priority] = NULL;

        // Decrement priority level until we find a non-empty queue
        do {
            g_priority--;
        } while(g_running[g_priority] == NULL);
    }
    else {

        // run the next task in the running queue
        g_running[g_priority] = g_running[g_priority]->next;

        // remove old proc from running queue
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;

    }

    // free old pcb
    free(temp);

    // Load new psp
    set_psp(g_running[g_priority]->psp);
    // Return to SVCall restores state

}


