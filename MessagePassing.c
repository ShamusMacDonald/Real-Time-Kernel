/*
 * MessagePassing.c
 *
 *  Created on: Sep 13, 2018
 *      Author: Shamus MacDonald
 *      Description: Contains the message passing software declarations
 *      that enables the hardware driven interrupts to pass messages to
 *      the monitor for processing, as well as pass output messages to the
 *      uart for display to the user.
 */

#include "MessagePassing.h"
#include "Interrupt.h"
#include "Systick.h"

#define PRIVATE static

/* Global                               *
 * Signals that the UART can transmit   */
extern volatile unsigned UART_IDLE;

/* InputQueue contains messages coming from either the UART or SYSTICK. *
 * The message info is received by the monitor.                         */
PRIVATE struct Queue InputQueue = {.capacity=QSIZE, .head=0, .tail=0, .size=0 };

/* OutputQueue contains messages coming from the monitor.                *
 * The message info is received by the UART which transmits the data     *
 * to the users terminal.                                                */
PRIVATE struct Queue OutputQueue = {.capacity=QSIZE, .head=0, .tail=0, .size=0 };

/* InitMsgPassing initializes the underlying hardware that allows the    *
 * message passing functionality. It must be called before any function  *
 * in MessagePassing.h is used                                           */
void InitMsgPassing(void)
{
    InitTerminal();
//    InitClock();
    InterruptMasterEnable();
}

/* SendMessage is the entry point to the Input and Output queues.         *
 * If the destination is UART0 then the message data will be either       *
 * forced out to the UART if the OutQ is empty, or it will be queued in   *
 * the OutQ.                                                              *
 * If the destination is the monitor, then the message will be queued in  *
 * the InQ.                                                               *
 * Returns 1 if message queued successfully or returns 0 if queue full    */
int SendMessage(char data, enum Sender src, enum Receiver dst)
{
    struct Message msg = { src, dst, data };
    struct Queue *Q = &InputQueue;

    // if receiver is the uart then use output q
    if(dst == UART0_RX) {
        // if uart idle then force char out
        if(UART_IDLE) {
            UART_PutChar(data);
            return 1;
        }
        // else add to output queue
        else {
            Q = &OutputQueue;
        }
    }
    // Check if Q full return 0
    if(Q->size == Q->capacity) {
        return 0;
    }
    // add message to Q
    Q->fifo[Q->tail] = msg;
    // adjust Q
    Q->tail = (Q->tail + 1) % Q->capacity;
    Q->size++;

    return 1;
}

/* ReceiveMessage is responsible for returning the data at the top of the    *
 * appropriate queue. If the receiver is UART, it removes the data from the  *
 * OutQ, else it removes from the InQ.                                       *
 * Returns 0 if the Q is empty, returns 1 if message received.               */
int ReceiveMessage(char *data, enum Sender *src, enum Receiver dst)
{
    struct Queue *Q = &InputQueue;

    // if receiver is the uart then use output q
    if(dst == UART0_RX) {
        Q = &OutputQueue;
    }
    // if q empty return 0
    if(Q->size == 0) {
        return 0;
    }
    // get msg info
    *data = Q->fifo[Q->head].data;
    *src = Q->fifo[Q->head].source;
    // adjust queue
    Q->head = (Q->head + 1) % Q->capacity;
    Q->size--;

    return 1;
}
