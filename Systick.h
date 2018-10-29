/*
 *  Systick.h
 *
 *  Created on: Sep 15, 2018
 *  Author:     Shamus MacDonald
 *  Description: Contains the software for handling Tiva
 *  SYSTICK exceptions.
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#define PRIVATE static

// SysTick Registers
// SysTick Control and Status Register (STCTRL)
#define ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))
// Systick Reload Value Register (STRELOAD)
#define ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))

// SysTick defines
#define ST_CTRL_COUNT      0x00010000  // Count Flag for STCTRL
#define ST_CTRL_CLK_SRC    0x00000004  // Clock Source for STCTRL
#define ST_CTRL_INTEN      0x00000002  // Interrupt Enable for STCTRL
#define ST_CTRL_ENABLE     0x00000001  // Enable for STCTRL

// Counter Periods
#define MAX_WAIT           0x1000000    // 2^24 = 16,777,216
#define ONE_SECOND         0x0F42400    // 16,000,000
#define TENTH_SEC          0x0186A00    // 1,600,000

// Entry points
void SysTickHandler(void);
void InitClock(void);

// Private Funcs
PRIVATE void SysTickStart(void);
PRIVATE void SysTickStop(void);
PRIVATE void SysTickPeriod(unsigned long);
PRIVATE void SysTickIntEnable(void);
PRIVATE void SysTickIntDisable(void);

#endif /* SYSTICK_H_ */
