/*
 * kMessaging.h
 *
 *  Created on: Oct 29, 2018
 *      Author: Shamus MacDonald
 */

#ifndef KMESSAGING_H_
#define KMESSAGING_H_

#define QSZ   64

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
    struct TCB *owner;
    unsigned capacity;
    unsigned size;
    unsigned head;
    unsigned tail;
    unsigned avail;

};

void init_msg(void);
int k_bind(int, int);
int k_send(int, int, const void *, unsigned);
int k_recv(unsigned, unsigned *, void *, unsigned);
int k_printcup(char, unsigned, unsigned);

#endif /* MESSAGING_H_ */
