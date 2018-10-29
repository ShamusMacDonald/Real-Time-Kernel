

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

#include "MessagePassing.h"
#include "process.h"
#include "KernelCalls.h"

#define COUNT_TM    0x0186A00

void print_a(void)
{
    int i;
    char id;

    print_list();

    while(1) {
        for(i=0; i<COUNT_TM;i++) ;
        id = t_getpid() + '0';
        print_ch('\n');
        print_ch(id);
        print_ch('\n');
        t_kill();
    }
}

void print_b(void)
{
    int i;
    char id;
    static unsigned count;

    while(1) {
        for(i=0; i<COUNT_TM;i++) ;
        id = t_getpid() + '0';
        print_ch('\n');
        print_ch(id);
        print_ch('\n');

        if(count++ >= 10) {
            break;
        }
    }
}

void print_c(void)
{
    int i;
    char id;
    while(1) {
        for(i=0; i<COUNT_TM;i++) ;
        id = t_getpid() + '0';
        print_ch('\n');
        print_ch(id);
        print_ch('\n');
        break;
    }
}

void idle_task(void)
{
    int i;
    char spinner[]={'/','-','\\','|'};
    unsigned index = 0;

    while(1) {

        for(i=0; i < COUNT_TM/2; i++) ;

        print_ch(spinner[index]);
        print_ch('\b');

        index = (index + 1) % 4;
    }
}

int main(void)
{

//    InitMsgPassing();

    // name, id, priority
    reg_proc(idle_task,0,0);
    reg_proc(print_a,1,5);
    reg_proc(print_b,2,4);
    reg_proc(print_c,3,3);
    reg_proc(print_c,4,1);
    reg_proc(print_b,5,2);

//    print_list();

    // Initialize thread mode and start first process
    SVC();

    while(1) {

    }

	return 0;
}
