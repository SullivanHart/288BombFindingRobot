/*
 * UART Functions
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

#include "uart.h"
#include "Timer.h"
#include <string.h>
#include "lcd.h"

volatile char uart_data;  // UART interrupt code can place read data here
volatile char flag;       // UART interrupt can update this flag

void uart_init(void){
    
    SYSCTL_RCGCUART_R |= 0x02;          // enable clock UART1 (page 344)
    timer_waitMillis(100);
    SYSCTL_RCGCGPIO_R |= 0x02;          // enable clock GPIOB (page 340)
    timer_waitMillis(100);

    GPIO_PORTB_AFSEL_R |= 0x03;          // sets PB0 and PB1 as peripherals (page 671)
    GPIO_PORTB_PCTL_R  |= 0x00000011;    // pmc0 and pmc1       (page 688)  also refer to page 650
    GPIO_PORTB_DEN_R   |= 0x03;          // enables pb0 and pb1
    GPIO_PORTB_DIR_R   |= 0x02;          // sets pb0 as input, pb1 as output

    //compute baud values [UART clock= 16 MHz]
    int fbrd;
    int ibrd;

    ibrd = (16000000.0 / (16.0 * 115200.0));
    fbrd = ((16000000.0 / (16.0 * 115200.0)) - ibrd) * 64.0 + 0.5; // page 903

    UART1_CTL_R &= 0xFFFE;      // disable UART1 (page 918)
    UART1_IBRD_R = ibrd;        // write integer portion of BRD to IBRD
    UART1_FBRD_R = fbrd;        // write fractional portion of BRD to FBRD
    UART1_LCRH_R |= 0x60;        // write serial communication parameters (page 916) * 8bit and no parity
    UART1_CC_R   |= 0x0;         // use system clock as clock source (page 939)
    UART1_CTL_R |= 0x0301;      // enable UART1
}

void uart_sendChar(char data){

    while(UART1_FR_R & 0x20){

    }

    //send data
    UART1_DR_R = data;
}

char uart_receive(void){

    while(1){
        if (flag)
        {
            flag = 0;
            return uart_data;
        }
    }
}

void uart_sendStr(const char *data){


    lcd_clear();
    lcd_puts((char*) data);

    int i;

    for (i = 0; i < strlen(data); i++){

        if (data[i] == '\0'){
            break;
        }

        uart_sendChar(data[i]);
    }

//    uart_sendChar('\r');
    uart_sendChar('\n');
}


void uart_interrupt_init()
{
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0x0010; //enable interrupt on receive - page 924

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0x00000040; //enable uart1 interrupts - page 104

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, uart_interrupt_handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number
}

void uart_interrupt_handler()
{
    // STEP1: Check the Masked Interrupt Status
    if (UART1_MIS_R & 0x0010) {
        flag = 1;
        //STEP2:  Copy the data
        uart_data = (char)(UART1_DR_R & 0xFF);
        //STEP3:  Clear the interrupt
        UART1_ICR_R = 0x10;
    }
}
