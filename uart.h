/*
*
*   uart.h
*
*   Used to set up the UART
*   uses UART1 at 115200
*
*
*   @author Dane Larson
*   @date 07/18/2016
*   Phillip Jones updated 9/2019, removed WiFi.h
*/

extern volatile char uart_data;  // Your UART interupt code can place read data here
extern volatile char flag;       // Your UART interupt can update this flag

#ifndef UART_H_
#define UART_H_

#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

void uart_init(void);

void uart_sendChar(char data);

char uart_receive(void);

void uart_sendStr(const char *data);

void uart_interrupt_init(void);

void uart_interrupt_handler(void);

#endif /* UART_H_ */
