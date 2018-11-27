/*
 *  kMessaging.c
 *
 *  Created on: Oct 28, 2018
 *      Author: Shamus MacDonald
 *      Description: Contains the message passing software that enables
 *      inter-process communication.
 *
 */

#include <hUART.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "kTaskSupport.h"
#include "kMessaging.h"
#include "kTask.h"

#define MAX_MQ  32
#define ERROR   -1

#define FALSE   0
#define TRUE    1

#define CUP_SZ      10
#define UART0_QID   0

static struct msg_q mq_list[MAX_MQ];

/*
 * Extern Globals
 */
extern struct TCB *g_running[PRI_LVLS];
extern unsigned g_priority;

// Signals that the UART can transmit
extern volatile unsigned UART_IDLE;

int k_bind(int qid, int istask)
{

    // verify user desired msg_q is a valid id and available
    if( ( qid >= 0 ) && ( qid < MAX_MQ ) && mq_list[qid].avail) {

        // bind to msg_q and return qid to user
        mq_list[qid].avail = FALSE;

        /* If a task is requesting a msgq, update the qid in the
         * tasks tcb
         */
        if(istask) {
            g_running[g_priority]->mqid = qid;
            mq_list[qid].owner = g_running[g_priority];
        }

        return qid;
    }
    else {
        return ERROR;
    }

}

/* SendMessage is the entry point to the Input and Output queues.         *
 * If the destination is UART0 then the message data will be either       *
 * forced out to the UART if the OutQ is empty, or it will be queued in   *
 * the OutQ.                                                              *
 * If the destination is the monitor, then the message will be queued in  *
 * the InQ.                                                               *
 * Returns 1 if message queued successfully or returns 0 if queue full    */
int k_send(int dst, int src, const void *data, unsigned size)
{
    struct msg msg = {.src = src, .size = size};

    /* Verify uart is only receiving chars
     * & that the src and dst are bound to queues
     */
    if( ((dst == UART0_QID) && (size > sizeof(char)))
        || (src < 0) || mq_list[dst].avail ) {
        return 0;
    }

    // allocate space for the msg in kspace
    msg.data = malloc(size);

    // copy the data into kspace memory
    memcpy(msg.data, data, size);

    if(( dst == UART0_QID ) && UART_IDLE ) {
        // if uart idle then force char out
        UART_PutChar(*((char *)data));
        return 1;
    }

    // Check if Q full return 0
    if(mq_list[dst].size == mq_list[dst].capacity) {
        UART_PutChar(*((char *)data));
        //    return 0;
    }

    // add message to Q
    mq_list[dst].fifo[mq_list[dst].tail] = msg;

    // adjust Q
    mq_list[dst].tail = (mq_list[dst].tail + 1) % mq_list[dst].capacity;
    mq_list[dst].size++;

    // If dst blocked, wake process up
    if( (dst != UART0_QID) && (mq_list[dst].owner->blocked) ) {

        enq_task(mq_list[dst].owner);

    }

    return 1;
}

/* ReceiveMessage is responsible for returning the data at the top of the    *
 * appropriate queue. If the receiver is UART, it removes the data from the  *
 * OutQ, else it removes from the InQ.                                       *
 * Returns 0 if the Q is empty, returns 1 if message received.               */
int k_recv(unsigned dst, unsigned *src, void *data, unsigned size)
{
    struct msg_q *mq;

    mq = &mq_list[dst];

    // if q empty block process
    if( mq->size <= 0 ) {

        if(dst != UART0_RX) {

            mq->owner->blocked = TRUE;
            rmv_task();

        }
        return 0;
    }

    // get msg info
    if(size < mq->fifo[mq->head].size) {
        return 0;
    }
    else {
        memcpy(data,mq->fifo[mq->head].data,size);
        *src = mq->fifo[mq->head].src;
    }

    // free allocated space for msg data in kspace
    free(mq->fifo[mq->head].data);

    // adjust queue
    mq->head = (mq->head + 1) % mq->capacity;
    mq->size--;

    return 1;
}

// Position of each VT100 CUP character
enum CUP_POS {  ESC, BRKT, ROW_TENS, ROW_ONES, SCOL,
                COL_TENS, COL_ONES, CMD, DATA };

int k_printcup(char data, unsigned row, unsigned col)
{
    int i;
    char uart_data[CUP_SZ];

    uart_data[ESC]      = '\e';
    uart_data[BRKT]     = '[';
    uart_data[ROW_TENS] = '0' + row / 10;
    uart_data[ROW_ONES] = '0' + row % 10;
    uart_data[SCOL]     = ';';
    uart_data[COL_TENS] = '0' + col / 10;
    uart_data[COL_ONES] = '0' + col % 10;
    uart_data[CMD]      = 'f';
    uart_data[DATA]     = data;
    uart_data[DATA+1]   = '\0';

    for(i = 0; i < CUP_SZ; i++) {

        k_send(UART0_QID, g_running[g_priority]->mqid,
               &uart_data[i], sizeof(uart_data[i]));

    }
    return 1;

}

void init_msg(void)
{
    unsigned i;

    for(i = 0; i < MAX_MQ; i++) {
        mq_list[i].avail = TRUE;
        mq_list[i].capacity = QSZ;
        mq_list[i].head=0;
        mq_list[i].tail=0;
        mq_list[i].size = 0;
        mq_list[i].owner = NULL;
    }

    // initialize UART serial comm
    InitTerminal();

    InterruptMasterEnable();


}

