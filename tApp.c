/*
 *  tApp.c
 *
 *  Created on: Nov 05, 2018
 *      Author: Shamus
 */

#include "tkCalls.h"
#include "tPrint.h"
#include "tApp.h"

#define CLR_SCREEN      "\e[2J"
#define CURSOR_HOME     "\e[H"
#define COLUMN_EIGHTY   80
#define BUF             12
#define TASK1_QID       5
#define TASK5_QID       4

void task1(void)
{
    int row, col = 1;
    char data;
    unsigned src;

    t_bind(5);
    row = t_getpid();

    print_str(CLR_SCREEN);
    print_str(CURSOR_HOME);


    while(1) {

    // Rx from UART
    if( t_recv(&src, &data, sizeof(data)) ) {

        // recv successful, print user input
        t_printcup(data, row, col++);

        // restart at col 1
        if (col > 80) {
            col = 1;
        }
    }

    }
}

void task2(void)
{
    int i;
    int row, col = 1;
    int pri;

    t_bind(10);
    row = t_getpid();

    while(1) {

        // delay
        for(i=0; i < COUNT_TM/16; i++) ;

        // get task priority
        pri = t_getpr();

        // print priority at specified cursor position
        t_printcup(pri + '0', row, col++);

        // reset colon after col 80
        if (col > COLUMN_EIGHTY) {

            col = 1;

            if( --pri > 0 ) {
                t_nice(pri);
            }
            else {
                break;
            }

        }
    }
}

void task3(void)
{
    int i, pri;
    int row, col = 1;

    t_bind(15);
    row = t_getpid();

    while(1) {

        // delay
        for(i=0; i < COUNT_TM/16; i++) ;

        // get priority
        pri = t_getpr();

        // print priority at specified cursor position
        t_printcup(pri + '0', row, col++);

        if (col > 80) {
            col = 1;

            // increment priority until 5
            if( --pri > 0 ) {
               t_nice(pri);
           }
            else {
                t_kill();
            }
        }
    }
}

void task4(void)
{

    char data[BUF] = "hello there";

    t_bind(20);

    t_send(TASK5_QID, &data, BUF);

    while(1) {
        break;
    }
}

void task5(void)
{
    char data[BUF];
    unsigned src;

    t_bind(4);
    t_getpid();

    print_str(CLR_SCREEN);
    print_str(CURSOR_HOME);

    while(1) {

       if( t_recv(&src, &data, BUF) ) {

            // recv successful, print task input
            print_str(data);

        }
    }
}

