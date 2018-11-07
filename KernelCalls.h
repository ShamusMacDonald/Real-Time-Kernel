/*
 *  KernelCalls.h
 *
 *  Created on: Oct 16, 2018
 *      Author: Shamus MacDonald
 *      Disclaimer: The declarations contained in this file and the code contained
 *      in the associated c file were edited to be used in this project
 *      from work obtained from Dr. Larry Hughes in ECED4402 - Real Time Systems
 *      Description: Responsible for handling supervisory calls to the kernel. The
 *      first call is responsible for initializing the first process stack pointer,
 *      subsequent calls are from processes during execution.
 */

#ifndef KERNELCALLS_H_
#define KERNELCALLS_H_

#include <stdint.h>

#define PRIVATE static

enum KernelCallCodes {GETID, NICE, KILL, BIND, SEND, RECV, GETPR, BADCODE};

struct KernelCallArgs
{
    unsigned code;
    unsigned rtnval;
    unsigned arg1;
    unsigned arg2;
    unsigned arg3;
};

// Kernel Calls
uint32_t t_getpid(void);
void t_kill(void);
int t_send(unsigned, const void *, unsigned);
int t_bind(int);
int t_getpr(void);

// Support Functions
void f_setR7(volatile uint32_t);


#endif /*KERNELCALLS_H_*/
