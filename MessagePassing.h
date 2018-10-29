/*
 *  MessagePassing.h
 *
 *  Created on: Sep 13, 2018
 *      Author: Shamus MacDonald
 *      Description: Contains the message passing software that enables
 *      the hardware driven interrupts to pass messages to the monitor
 *      for processing, as well as pass output messages to the uart for
 *      display to the user.
 */

#ifndef MESSAGEPASSING_H_
#define MESSAGEPASSING_H_

// Num of msgs Qs can hold
#define QSIZE   64

// Message Passing Sources
enum Sender { UART0_TX, SYSTICK_TX, MONITOR_TX };
// Message Passing Destinations
enum Receiver { UART0_RX, MONITOR_RX };

/* Each Message contains the SENDER info,       *
 * the RECEIVER info, and the data being sent   */
struct Message {
    enum Sender source;
    enum Receiver destination;
    char data;
};

/* Queue Data Structure acts as a buffer *
 * for the message passing software      */
struct Queue {
    struct Message fifo[QSIZE];
    unsigned capacity;
    unsigned size;
    unsigned head;
    unsigned tail;
};

/* InitMsgPassing must be called before using any   *
 * messaging passing features                       */
void InitMsgPassing(void);
// Methods for message passing
int ReceiveMessage(char *, enum Sender *, enum Receiver);
int SendMessage(char, enum Sender, enum Receiver);

#endif /* MESSAGEPASSING_H_ */
