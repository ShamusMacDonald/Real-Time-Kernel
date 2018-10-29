/*
 * messaging.h
 *
 *  Created on: Oct 29, 2018
 *      Author: Shamus MacDonald
 *      Description:
 */

#ifndef MESSAGING_H_
#define MESSAGING_H_

#include <stdint.h>
#include "KernelCalls.h"

#define QSIZE   8

/* Each Message contains  info identifying the sender (from)
 * the size of the data, and the data being sent.
 */
struct msg {
    uint16_t src;
    uint8_t size;   // 0 - 255 bytes
    void *data;
};

/* MsgQueue
 * Each process can bind to a MsgQueue, once bound, the process
 * can send messages to other MsgQueues or check its own MsgQueue
 * for received messages.
 */
struct msg_q {
    struct msg fifo[QSIZE];
    unsigned capacity;
    unsigned size;
    unsigned head;
    unsigned tail;
    unsigned avail;
};

void init_msg(void);
void k_bind(struct KernelCallArgs *);
int k_send(uint16_t, uint16_t, const void *, uint8_t);
int k_recv(uint16_t, uint16_t *, void *, uint8_t);

#endif /* MESSAGING_H_ */
