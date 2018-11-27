/*
 * kPendSV.h
 *
 *  Created on: Nov 08, 2018
 *      Author: Shamus
 */

#ifndef KPENDSV_H_
#define KPENDSV_H_

#define _disable()   __asm(" cpsid i")
#define _enable()    __asm(" cpsie i")

#define NVIC_INT_CTRL_R (*((volatile unsigned long *) 0xE000ED04))
#define TRIGGER_PENDSV  0x10000000UL

#define NVIC_SYS_PRI2_R         (*((volatile unsigned long *) 0xE000ED1C))
#define SVC_PRI_FIVE            0xA0000000UL

#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *) 0xE000ED20))
#define PENDSV_PRI_SEVEN        0x00E00000UL
#define SYSTICK_PRI_SIX         0xC0000000UL

void PendSV_handler(void);
void init_priorities(void);

#endif /* KPENDSV_H_ */
