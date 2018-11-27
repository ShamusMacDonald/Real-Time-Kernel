/*
 * kTaskSupport.c
 *
 *  Created on: Nov 05, 2018
 *      Author: Shamus
 */

#include <stdint.h>
#include <stdlib.h>

#include "kTaskSupport.h"
#include "kTask.h"

#define FALSE 0

extern struct TCB *g_running[PRI_LVLS];
extern uint32_t g_priority;

struct TCB * rmv_task(void)
{
    struct TCB *temp;

    temp = g_running[g_priority];

    // Is this the last task in the queue?
    if(g_running[g_priority] == g_running[g_priority]->next) {

       // Set this queue as empty
       g_running[g_priority] = NULL;

       // Decrement priority level until we find a non-empty queue
       do {

           g_priority--;

       } while(g_running[g_priority] == NULL);

    }
    else {

       // run the next task in the running queue
       g_running[g_priority] = g_running[g_priority]->next;

       // remove old proc from running queue
       temp->prev->next = temp->next;
       temp->next->prev = temp->prev;

    }

    return temp;
}

void enq_task(struct TCB *p_tcb)
{
    unsigned pri = p_tcb->priority;

    if(g_running[pri] == NULL) {

        g_running[pri] = p_tcb;
        g_running[pri]->next = g_running[pri];
        g_running[pri]->prev = g_running[pri];

    }
    else {

        // Add to linked list
        p_tcb->next = g_running[pri]->next;

        p_tcb->prev = g_running[pri];
        p_tcb->next->prev = p_tcb;

        g_running[pri]->next = p_tcb;
        g_running[pri] = p_tcb;

    }

    // Set global priority if new proc pri highest
    if(pri > g_priority) {

        // update global pri
        g_priority = pri;

    }

    p_tcb->blocked = FALSE;

}

