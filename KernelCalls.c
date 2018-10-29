/*
 *  KernelCalls.c
 *
 *  Created on: Oct 17, 2018
 *      Author: Shamus MacDonald
 *      Disclaimer: The declarations contained in this file and the code contained
 *      in the associated c file were edited to be used in this project
 *      from work obtained from Dr. Larry Hughes in ECED4402 - Real Time Systems
 *      Description: Responsible for handling supervisory calls to the kernel. The
 *      first call is responsible for initializing the first process stack pointer,
 *      subsequent calls are from processes during execution.
 */

#include <stdint.h>
#include "KernelCalls.h"
#include "process.h"


/* t_kill is responsible for setting up the
 * necessary arguments for a kernel call via SVC
 * that is responsible for terminating the calling
 * process.
 */
void t_kill(void)
{
    // Setup arguments for kernel on process stack
    volatile struct KernelCallArgs kill_arg;

    kill_arg.code = KILL;

    // Set R7 to address of kernel argument
    f_setR7( (uint32_t)&kill_arg );

    // Call kernel
    SVC();
}

uint32_t t_getpid(void)
{
    // Setup arguments for kernel on process stack
    volatile struct KernelCallArgs getid_arg;

    getid_arg.code = GETID;

    // Set R7 to address of kernel argument
    f_setR7( (uint32_t)&getid_arg );

    // Call kernel
    SVC();

    // Kernel sets return value
    return getid_arg.rtnval;
}

//uint32_t f_tkcall()

void f_setR7(volatile uint32_t data)
{
    /* Set 'data' to R7; since the first argument is R0, this is
     * simply a MOV from R0 to R7
     */
    __asm("     mov r7,r0");
}
