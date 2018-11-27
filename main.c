
/* main.c
 * 
 * Coding style used throughout this project
 *		Variables:			
 *			Local: 		snake_case
 *			Pointers: 	p_snake_case
 *			Globals:	g_snake_case
 *		Functions:
 *			Public:		snake_case(args)
 *			Private:	f_snake_case(args)
 *		file:
*           task space:     tFileName
*           kernel space:   kFileName
*           tk interface:   tkFileName
 *		Other:
 *			Typedef:	snake_case_t
 *			defines:	UPPER_SNAKE_CASE
 *			enums:		UPPER_SNAKE_CASE 
 */

#include <stdint.h>
#include <stdio.h>

#include "kTask.h"
#include "kPendSV.h"
#include "tkCalls.h"
#include "tPrint.h"
#include "tApp.h"

#define ROW_1   1
#define COL_80  80

void idle_task(void)
{
    int i;
    const char spinner[]={'|','/','-','\\'};
    unsigned index = 0;

    t_bind(1);

    while(1) {

        for(i=0; i < COUNT_TM/8; i++) {
            ;
        }

        t_printcup(spinner[index], ROW_1, COL_80);

       index = (index + 1) % sizeof(spinner);
    }
}

#define TX_RX 0
//#define REG_TEST 0

int main(void)
{

    //       name,      id, priority
    reg_task(idle_task, 0,  0);

    #ifdef REG_TEST
    //       name,      id, priority
    reg_task(task1,     5,  5);
    reg_task(task2,     10, 5);
    reg_task(task3,     15, 5);
    #endif

    #ifdef TX_RX
    //       name,      id, priority
    reg_task(task4, 20, 1);     // sender
    reg_task(task5, 25, 5);     // receiver
    #endif


    // Initialize thread mode and start first process
    SVC();

	return 0;
}
