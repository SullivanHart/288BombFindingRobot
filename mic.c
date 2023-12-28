/*
*
*   mic.c
*/

#include "mic.h"
#include "lcd.h"
#include "Timer.h"


void mic_init()
{
    //enable clock/pins
    SYSCTL_RCGCGPIO_R |= 0b1;   // enable clock GPIOA
    SYSCTL_RCGCSSI_R |= 0b1;    //enable clock for ssi0
    GPIO_PORTA_AFSEL_R |= 0b111100;     // sets pa2:pa5 as peripheral
    GPIO_PORTA_DEN_R |= 0b111100;   // enables pa2:pa5
    GPIO_PORTA_DIR_R |= 0b01000;    // sets pb3 as out
    GPIO_PORTA_DIR_R &= 0b01011;    // sets pb4 & pb2 as in

    //set functionality of pins
    GPIO_PORTA_PCTL_R &= 0xFF0000FF;
    GPIO_PORTA_PCTL_R |= 0x00222200;

    //start with pa3 high (mic off)
    GPIO_PORTA_DATA_R |= 0b1000;

    //initialize ssi:
    SSI0_CR1_R &= 0b000;                //disable ssi0, config as master
    SSI0_CR0_R = 0b11001111;            //set ssi functions (clock rate, first or second edge, clk polarity when no data is transfered, frame format, data size)
                                        //0 SCR, 2nd edge clock transition, steady high, Freescale SPI, 16-bit data
    SSI0_CPSR_R = 0x4;                  //set clock divisor, (16MHz / (4 * (1 + 0)) = 4MHz (within 10Khz to 20MHz range)
    SSI0_CC_R &= 0x0;                   //choose clock source
    SSI0_CR1_R |= 0b10010;              //enable & set end of transmission interrupt

    //connect slave select (pa3) to ss: output? (low when communicating, high when not)
    //94v-0 is not needed
    //connect the ssi0x (receive) (pa4) to miso: input
    //connect ssi clock (pa2) to sck: output
    //gnd to gnd
    //connect 3.3v to vcc
}

int mic_receive()
{
    int data = 7;

    GPIO_PORTA_DATA_R &= ~0b1000;         //turn off pa3 (turn on mic)

    while (SSI0_SR_R & 0b010000){       //wait until fifo isn't busy
    }

    GPIO_PORTA_DATA_R |= 0b1000;         //turn pa3 back on (turn off mic)

    data = (int)(SSI0_DR_R & 0xFFFF);   //get and store data

    return data;
 
}

void mic_interrupt_init()
{
    // Enable interrupts for receiving bytes through SSI0
    SSI0_IM_R |= 0b100; //enable interrupt on receive

    // Find the NVIC enable register and bit responsible for SSI0 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0b010000000; //enable ssi0 interrupts - #7

    IntRegister(INT_SSI0, mic_interrupt_handler); //give the microcontroller the address of our interrupt handler - #23
}

void mic_interrupt_handler()
{
    // STEP1: Check the Masked Interrupt Status
    if (SSI0_RIS_R & 0b100){ //check for Receive FIFO interrupt
    } else {
        return;
    }

    //STEP2:  Copy the data
    mic_data = SSI0_DR_R & 0xFFFF;
    micFlag++;

    //STEP3:  Clear the interrupt
    SSI0_ICR_R = 0b111;
}
