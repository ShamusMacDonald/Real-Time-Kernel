/*
 *  tkCalls.h
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

#ifndef TKCALLS_H_
#define TKCALLS_H_

#include <stdint.h>

#define PRIVATE static

enum KernelCallCodes {GETID, NICE, KILL, BIND, SEND,
                      RECV, GETPR, PRINT_CUP , BADCODE};

struct KernelCallArgs
{
    unsigned code;
    unsigned rtnval;
    void *args;
};

struct tkmsg
{
    void *data;
    unsigned size;

    /* node can be a dst or src
     * so struct can be used by both
     * recv and send
     */
    unsigned node;
};

struct PrintCup
{
    unsigned row;
    unsigned col;
    char data;
};

// Kernel Calls
unsigned t_getpid(void);
void t_kill(void);
int t_send(unsigned, void *, unsigned);
int t_recv(unsigned *, void *, unsigned);
int t_bind(int);
int t_getpr(void);
int t_nice(int);
int t_printcup(char, unsigned, unsigned);
int tkcall(int, void *);

// Support Functions
void f_setR7(volatile uint32_t);


#endif /*KERNELCALLS_H_*/
