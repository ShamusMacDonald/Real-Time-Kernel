/*
 *  messaging.c
 *
 *  Created on: Oct 28, 2018
 *      Author: Shamus MacDonald
 *      Description: Contains the message passing software that enables
 *      inter-process communication.
 *
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "messaging.h"
#include "KernelCalls.h"
#include "Interrupt.h"
#include "process.h"

#define MAX_MQ  16
#define ERROR   -1

#define FALSE   0
#define TRUE    1

static struct msg_q mq_list[MAX_MQ];

/*
 * Extern Globals
 *
 */
extern struct PCB *g_running[PRI_LVLS];
extern uint32_t g_priority;

// Signals that the UART can transmit
extern volatile unsigned UART_IDLE;

void k_bind(struct KernelCallArgs *args)
{
    // verify user desired msg_q is available
    if(( args->arg1 < MAX_MQ ) && mq_list[args->arg1].avail) {
        // bind to msg_q and return mqid to user
        mq_list[args->arg1].avail = FALSE;
        g_running[g_priority]->mqid = args->arg1;
        args->rtnval = args->arg1;
    }
    else {
        args->rtnval = ERROR;
    }
}

/* SendMessage is the entry point to the Input and Output queues.         *
 * If the destination is UART0 then the message data will be either       *
 * forced out to the UART if the OutQ is empty, or it will be queued in   *
 * the OutQ.                                                              *
 * If the destination is the monitor, then the message will be queued in  *
 * the InQ.                                                               *
 * Returns 1 if message queued successfully or returns 0 if queue full    */
int k_send(unsigned dst, unsigned src, const void *data, unsigned size)
{
    struct msg msg = {.src = src, .size = size};

    if( (dst == UART0_RX) && (size > sizeof(char)) ) {
        return 0;
    }

    // allocate space for the msg in kspace
    msg.data = malloc(size);

    // copy the data into kspace memory
    memcpy(msg.data, data, size);

    if(( dst == UART0_RX ) && UART_IDLE ) {
        // if uart idle then force char out
        UART_PutChar(*((char *)data));
        return 1;
    }

    // Check if Q full return 0
    if(mq_list[dst].size == mq_list[dst].capacity) {
        return 0;
    }

    // add message to Q
    mq_list[dst].fifo[mq_list[dst].tail] = msg;

    // adjust Q
    mq_list[dst].tail = (mq_list[dst].tail + 1) % mq_list[dst].capacity;
    mq_list[dst].size++;

    return 1;
}

/* ReceiveMessage is responsible for returning the data at the top of the    *
 * appropriate queue. If the receiver is UART, it removes the data from the  *
 * OutQ, else it removes from the InQ.                                       *
 * Returns 0 if the Q is empty, returns 1 if message received.               */
int k_recv(unsigned dst, unsigned *src, void *data, unsigned size)
{
    // if q empty return 0
    if(mq_list[dst].size == 0) {
        return 0;
    }

    // get msg info
    if(size < mq_list[dst].fifo[mq_list[dst].head].size) {
        return 0;
    }
    else {
        memcpy(data,mq_list[dst].fifo[mq_list[dst].head].data,size);
        *src = mq_list[dst].fifo[mq_list[dst].head].src;
    }

    // free allocated space for msg data in kspace
    free(mq_list[dst].fifo[mq_list[dst].head].data);

    // adjust queue
    mq_list[dst].head = (mq_list[dst].head + 1) % mq_list[dst].capacity;
    mq_list[dst].size--;

    return 1;
}

void init_msg(void)
{
    unsigned i;

    // initialize UART serial comm
    InitTerminal();
    InterruptMasterEnable();

    // bind UART0 to mqid 0
    mq_list[UART0_RX].avail = FALSE;
    mq_list[UART0_RX].capacity = QSZ;
    mq_list[UART0_RX].head=0;
    mq_list[UART0_RX].tail=0;
    mq_list[UART0_RX].size=0;

    // initialize each mq_list
    for(i = 1; i < MAX_MQ; i++) {
        mq_list[i].avail = TRUE;
        mq_list[i].capacity = QSZ;
        mq_list[i].head=0;
        mq_list[i].tail=0;
        mq_list[i].size=0;
    }
}

/*
int k_send(unsigned to, char *msg, unsigned size)
{

}
*/


