/*
 * kTaskSupport.h
 *
 *  Created on: Nov 05, 2018
 *      Author: Shamus
 */

#ifndef KTASKSUPPORT_
#define KTASKSUPPORT_

#include "kTask.h"


/* rmv_task
 * Removes the currently running process
 * from the running linked list and sets up
 * the next process to run.
 * Returns a pointer to the removed process.
 * The caller is responsible for setting the new
 * running process's psp, as well as handling the
 * removed process.
 */
struct TCB * rmv_task(void);

/* enq_task
 * Enqueues the argument task to the running list
 * of the appropriate priority.
 */
void enq_task(struct TCB *);



#endif /* KERNEL_SUPPORT_H_ */
