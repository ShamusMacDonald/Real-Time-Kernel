/*
 *  Systick.c
 *
 *  Created on: Sep 15, 2018
 *      Author: Shamus MacDonald
 *      Disclaimer: The code contained in this file and the declarations contained
 *      in the associated header file were edited to be used in this project
 *      from work obtained from Dr. Larry Hughes in ECED4402 - Real Time Systems
 *      Description: Contains the software for handling Tiva
 *      SYSTICK exceptions.
 */

#include "Systick.h"
#include "process.h"

/************************************************************
************************ ENTRY POINTS ***********************
*************************************************************/

extern struct PCB *g_running[PRI_LVLS];
extern uint32_t g_priority;

/* InitClock is the entry point to initializing the system timer.    *
 * Call to enable systick interrupts every 1/10th second.            */
void InitClock(void)
{
    // Initialize SYSTICK
    SysTickPeriod(ONE_SECOND);
    SysTickIntEnable();
    SysTickStart();
}

/* SysTickHandler handles SYSTICK interrupts. The purpose of SYSTICK *
 * interrupts is to perform context switches between processes.      *
 * The SYSTICK handler assumes that the system has initialized all   *
 * running processes beforehand, using the interface provided by     *
 * process.h.                                                        */
void SysTickHandler(void)
{
    // Save R4 - R11 on process stack
    save_registers();

    // Update current PCB psp
    g_running[g_priority]->psp = get_psp();

    // Move to next pcb
    g_running[g_priority] = g_running[g_priority]->next;
    // Set CPU psp
    set_psp(g_running[g_priority]->psp);

    // Load PCB state
    restore_registers();

}

/************************************************************
************************ PRIVATE FUNCS **********************
*************************************************************/
// SysTickStart enables the system timer to interrupt
void SysTickStart(void)
{
    // Set the clock source to internal and enable the counter to interrupt
    ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

/* SysTickPeriod sets the SYST_RELOAD register to the 24bit value Period   *
 * parameter. The clock speed is 16MHz, use this to determine timer value. */
void SysTickPeriod(unsigned long Period)
{
    // For an interrupt, must be between 2 and 16777216 (0x100.0000 or 2^24)
    ST_RELOAD_R = Period - 1;  // 1 to 0xff.ffff
}

// SysTickIntEnable sets the appropriate bits in the ctrl reg
void SysTickIntEnable(void)
{
    // Set the interrupt bit in STCTRL
    ST_CTRL_R |= ST_CTRL_INTEN;
}
