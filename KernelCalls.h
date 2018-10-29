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

enum KernelCallCodes {GETID, NICE, KILL, BIND, BADCODE};

struct KernelCallArgs
{
    unsigned int code;
    unsigned int rtnval;
    unsigned int arg1;
    unsigned int arg2;
};

// Kernel Calls
uint32_t t_getpid(void);
void t_kill(void);

// Support Functions
void f_setR7(volatile uint32_t);


#endif /*KERNELCALLS_H_*/
