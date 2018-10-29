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

#define QSZ   8

/* Each Message contains  info identifying the sender (from)
 * the size of the data, and the data being sent.
 */
struct msg {
    unsigned src;
    unsigned size;   // 0 - 255 bytes
    void *data;
};

/* MsgQueue
 * Each process can bind to a MsgQueue, once bound, the process
 * can send messages to other MsgQueues or check its own MsgQueue
 * for received messages.
 */
struct msg_q {
    struct msg fifo[QSZ];
    unsigned capacity;
    unsigned size;
    unsigned head;
    unsigned tail;
    unsigned avail;
};

void init_msg(void);
void k_bind(struct KernelCallArgs *);
int k_send(unsigned, unsigned, const void *, unsigned);
int k_recv(unsigned, unsigned *, void *, unsigned);

#endif /* MESSAGING_H_ */
