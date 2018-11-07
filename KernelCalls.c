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

/* TODO:
 * - Add pkcall functionality
 */

int t_getpr(void)
{
    // Setup arguments for kernel on process stack
   volatile struct KernelCallArgs getpr_arg;

   getpr_arg.code = GETPR;

   // Set R7 to address of kernel argument
   f_setR7( (uint32_t)&getpr_arg );

   // Call kernel
   SVC();

   // Kernel sets return value
   return getpr_arg.rtnval;
}

int t_bind(int qid)
{
    // Setup arguments for kernel on process stack
    volatile struct KernelCallArgs bind_arg;

    bind_arg.code = BIND;
    bind_arg.arg1 = qid;

    // Set R7 to address of kernel argument
    f_setR7( (uint32_t)&bind_arg );

    // Call kernel
    SVC();

    return bind_arg.rtnval;
}

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

int t_send(unsigned dst, const void *data, unsigned size)
{
    volatile struct KernelCallArgs send_args;

    send_args.code = SEND;
    send_args.arg1 = (unsigned)data;
    send_args.arg2 = size;
    send_args.arg3 = dst;

    // Set R7 to address of kernel argument
    f_setR7( (uint32_t)&send_args );

    // Call kernel
    SVC();

    return send_args.rtnval;

}

//int t_recv()

void f_setR7(volatile uint32_t data)
{
    /* Set 'data' to R7; since the first argument is R0, this is
     * simply a MOV from R0 to R7
     */
    __asm("     mov r7,r0");
}
