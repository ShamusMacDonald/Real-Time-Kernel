/*
 *  Interrupt.c
 *
 *  Created on: Sep 11, 2018
 *      Author: Shamus MacDonald
 *      Disclaimer: The code contained in this file and the declarations contained
 *      in the associated header file were edited to be used in this project
 *      from work obtained from Dr. Larry Hughes in ECED4402 - Real Time Systems
 *      Description: Contains the software for handling Tiva
 *      UART interrupts.
 */

#include "MessagePassing.h"
#include "Interrupt.h"
#include "messaging.h"

#define FALSE   0
#define TRUE    1

volatile unsigned UART_IDLE = TRUE;

/************************************************************
************************ ENTRY POINTS ***********************
*************************************************************/

/* InitTerminal enables data transfer between the users monitor at    *
 * BaudRate 115200 through Tiva UART0.                                */
void InitTerminal(void)
{
    UART0_Init();                               // Initialize UART0
    InterruptEnable(INT_VEC_UART0);             // Enable UART0 interrupts
    UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts
}

// InterruptMasterEnable call to enable interrupts
void InterruptMasterEnable(void)
{
    // enable CPU interrupts
    __asm(" cpsie   i");
}

/* UART_PutChar is the entry point for the message passing software  *
 * to output data to the users terminal.                             */
void UART_PutChar(char data)
{
    UART0_DR_R = data;
    UART_IDLE = FALSE;
}

/* UART0_IntHandler is responsible for handling UART0 interrupts,         *
 * as the name suggests. It is responsible for calling message passing    *
 * software to get the received data to the monitor or push data from the *
 * monitor out of the UART.                                               */
void UART0_IntHandler(void)
{
    char data;
    enum Sender src;

    if (UART0_MIS_R & UART_INT_RX)
    {
        // RECV done - clear interrupt and make char available to application
        UART0_ICR_R |= UART_INT_RX;
        SendMessage(UART0_DR_R, UART0_TX, MONITOR_RX);
    }

    if (UART0_MIS_R & UART_INT_TX)
    {
        // XMIT done - clear interrupt
        UART0_ICR_R |= UART_INT_TX;
        // Check if anymore chars in output queue
        if(ReceiveMessage(&data,&src,UART0_RX)) {
            // chars waiting so send through uart
            UART0_DR_R = data;
        }
        else {
            // Empty Q so uart now idle
            UART_IDLE = TRUE;
        }
    }
}

/************************************************************
************************ PRIVATE FUNCS **********************
*************************************************************/

/* UART0_Init enables clock gating for UART0, sets the baud rate to       *
 * 115200, sets word length to 8 bits w/ 1 stop bit, enables UART0 tx rx, *
 * and sets the UART ctrl reg.                                            */
void UART0_Init(void)
{
    volatile int wait;

    // Initialize UART0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOA;   // Enable Clock Gating for PORTA
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART0;   // Enable Clock Gating for UART0
    wait = 0; // give time for the clocks to activate

    UART0_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    UART0_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART0_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556

    UART0_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    GPIO_PORTA_AFSEL_R = 0x3;                           // Enable Receive and Transmit on PA1-0
    GPIO_PORTA_PCTL_R = (0x01) | ((0x01) << 4);         // Enable UART RX/TX pins on PA1-0
    GPIO_PORTA_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;         // Enable Digital I/O on PA1-0

    UART0_CTL_R = UART_CTL_UARTEN;  // Enable the UART
    wait = 0;                       // wait; give UART time to enable itself.
}

// InterruptEnable enables the interrupt by index in the NVIC
void InterruptEnable(unsigned long InterruptIndex)
{
    // Indicate to CPU which device is to interrupt
    if(InterruptIndex < 32)
        NVIC_EN0_R = 1 << InterruptIndex;           // Enable the interrupt in the EN0 Register
    else
        NVIC_EN1_R = 1 << (InterruptIndex - 32);    // Enable the interrupt in the EN1 Register
}

/* UART0_IntEnable sets the bits in the UART0IM register corresponding  *
 * to the flags parameter.                                              *
 * The set bits allow the corresponding interrupt signal to be routed   *
 * to the interrupt controller                                          */
void UART0_IntEnable(unsigned long flags)
{
    // Set specified bits for interrupt
    UART0_IM_R |= flags;
}
