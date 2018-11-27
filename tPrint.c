/*
 * tPrint.c
 *
 *  Created on: Oct. 31, 2018
 *      Author: Shamus
 */

#include <string.h>

#include "tPrint.h"
#include "tkCalls.h"

#define UART0_QID   0

/* PrintChar sends a message to the UART with the data  *
 * passed to the function.                              */
void print_ch(char ch)
{
    t_send(UART0_QID, &ch, sizeof(ch));
}

// PrintString calls PrintChar for each char in the string
void print_str(char *str)
{
    int i = 0;
    while( i < strlen(str) ) {
        print_ch(str[i]);
        i++;
    }
}
