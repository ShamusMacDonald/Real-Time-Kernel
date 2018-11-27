/*
 * kPendSV.c
 *
 *  Created on: Nov 08, 2018
 *      Author: Shamus
 */

#include "kPendSV.h"
#include "kTask.h"

extern struct TCB *g_running[PRI_LVLS];
extern unsigned g_priority;

void init_priorities(void)
{
    // PendSV priority 7 (lowest priority)
    NVIC_SYS_PRI3_R |= PENDSV_PRI_SEVEN;

    // Systick priority 6
    NVIC_SYS_PRI3_R |= SYSTICK_PRI_SIX;

    // SVC priority 5
//    NVIC_SYS_PRI2_R |= SVC_PRI_FIVE;

}

void PendSV_handler(void)
{
    /* disable interrupts so cannot be interrupted
     * by high priority interrupts
     */
    _disable();

    // Save R4 - R11 on process stack
    save_registers();

    // Update current TCB psp
    g_running[g_priority]->psp = get_psp();

    // Move to next tcb
    g_running[g_priority] = g_running[g_priority]->next;

    // Set CPU psp
    set_psp(g_running[g_priority]->psp);

    // Load PCB state
    restore_registers();

    // Enable interrupts
    _enable();
}



