/*
 *  tkcalls.c
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

#include "tkCalls.h"
#include "kTask.h"

#define NULL 0

int t_getpr(void)
{
   return tkcall(GETPR, NULL);
}

int t_bind(int qid)
{
    return tkcall(BIND, &qid);
}

/* t_kill is responsible for setting up the
 * necessary arguments for a kernel call via SVC
 * that is responsible for terminating the calling
 * process.
 */
void t_kill(void)
{
    tkcall(KILL, NULL);
}

unsigned t_getpid(void)
{
    return tkcall(GETID, NULL);
}

/* nice
 * nice is responsible for changing the priority
 * of the calling task. The new priority must be
 * a value 1 through 5. Returns the new priority if
 * successful else returns -1.
 */
int t_nice(int newpr)
{
    return tkcall(NICE, &newpr);
}

int t_send(unsigned dst, void *data, unsigned size)
{
    struct tkmsg msg_args;

    // Setup arguments
    msg_args.data = data;
    msg_args.size = size;
    msg_args.node = dst;

    return tkcall(SEND, &msg_args);

}

int t_recv(unsigned *src, void *data, unsigned size)
{
    struct tkmsg msg_args;

     // Setup arguments
     msg_args.data = data;
     msg_args.size = size;
     msg_args.node = (unsigned)src;

     // if recv successful
     return tkcall(RECV, &msg_args);

}

/* Output a single character to specified screen position */
/* CUP (Cursor position) command plus character to display */
/* Assumes row and col are valid (1 through 24 and 1 through 80, respectively) */
int t_printcup(char data, unsigned row, unsigned col)
{
    struct PrintCup uart_data;

    uart_data.row = row;
    uart_data.col = col;
    uart_data.data = data;

    return tkcall(PRINT_CUP, &uart_data);
}

int tkcall(int code, void *args)
{
    volatile struct KernelCallArgs kern_args;

    kern_args.code = code;
    kern_args.args = args;

    f_setR7( (uint32_t)&kern_args );

    SVC();

    return kern_args.rtnval;
}

void f_setR7(volatile uint32_t data)
{
    /* Set 'data' to R7; since the first argument is R0, this is
     * simply a MOV from R0 to R7
     */
    __asm("     mov r7,r0");
}
