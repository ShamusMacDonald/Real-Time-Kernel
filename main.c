

/* main.c
 * 
 * Coding style used throughout this project
 *		Variables:			
 *			Local: 		snake_case
 *			Pointers: 	p_snake_case
 *			Globals:	g_snake_case
 *		Functions:
 *			Public:		snake_case( args )
 *			Private:	f_snake_case(args)
 *		Other:
			Structs:	camelCase		
 *			Typedef:	snake_case_t
 *			defines:	UPPER_SNAKE_CASE
 *			enums:		UPPER_SNAKE_CASE 
 */

#include <stdint.h>
#include <stdio.h>

#include "process.h"
#include "KernelCalls.h"

#define COUNT_TM    0x0186A00

void print_a(void)
{
    static int sqid = 15;
    int i, id, qid, pr;
    char buf[48];

    qid = t_bind(sqid++);

    while(1) {
        for(i=0; i<COUNT_TM;i++) ;

        id = t_getpid();
        pr = t_getpr();

        snprintf(buf,48,"\nid: %u\n"
                 "qid: %u\n"
                 "pr: %u\n", id, qid, pr);
        print_str(buf);

        t_kill();
    }
}

void print_b(void)
{
    static int sqid = 3;
    static unsigned count;
    int i, id, qid, pr;
    char buf[32];

    qid = t_bind(sqid++);

    while(1) {
        for(i=0; i<COUNT_TM;i++) ;

        id = t_getpid(); //+ '0';
        pr = t_getpr();

        snprintf(buf,32,"\nid: %u\n"
                 "qid: %u\n"
                 "pr: %u\n", id, qid, pr);

        print_str(buf);

        if(count++ >= 10) {
            break;
        }
    }
}

void print_c(void)
{
    static int sqid = 8;
    int i, id, pr, qid;
    char buf[32];

    qid = t_bind(sqid++);

    while(1) {
        for(i=0; i<COUNT_TM;i++) ;
        id = t_getpid(); //+ '0';
        pr = t_getpr();

        snprintf(buf,32,"\nid: %u\n"
                 "qid: %u\n"
                 "pr: %u\n", id, qid, pr);

        print_str(buf);

        break;
    }
}

void idle_task(void)
{
    int i;
    char spinner[]={'/','-','\\','|'};
    unsigned index = 0;

    t_bind(1);

    while(1) {

        for(i=0; i < COUNT_TM/2; i++) ;

        print_ch(spinner[index]);
        print_ch('\b');

        index = (index + 1) % 4;
    }
}

int main(void)
{
    // name, id, priority
    reg_proc(idle_task,0,0);
    reg_proc(print_a,1,5);
    reg_proc(print_b,2,4);
    reg_proc(print_c,3,3);
    reg_proc(print_c,4,1);
    reg_proc(print_b,5,2);

    // Initialize thread mode and start first process
    SVC();

	return 0;
}
