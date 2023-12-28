/*
 * Button Header
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include "driverlib/interrupt.h"

extern volatile int button_event;
extern volatile int button_num;

volatile int button_num;
//initialize the push buttons
void button_init();

// Initialize GPIO interrupts for buttons
void init_button_interrupts();

// handler for gpio event when button is pressed
void gpioe_handler();

///Non-blocking call
///Returns highest value button being pressed, 0 if no button pressed
uint8_t button_getButton();


#endif /* BUTTON_H_ */
