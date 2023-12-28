/*
 * Button functionality on CyBot
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */


//The buttons are on PORTE 3:0
// GPIO_PORTE_DATA_R -- Name of the memory mapped register for GPIO Port E, 
// which is connected to the push buttons
#include "button.h"

// Global varibles
volatile int button_event;
volatile int button_num;

/**
 * Initialize PORTE and configure bits 0-3 to be used as inputs for the buttons.
 */
void button_init() {
	static uint8_t initialized = 0;

	//Check if already initialized
	if(initialized){
		return;
	}

    // 1) Turn on PORTE system clock, do not modify other clock enables
    SYSCTL_RCGCGPIO_R |= 0b00010000;

    // 2) Set the buttons as inputs, do not modify other PORTE wires
    GPIO_PORTE_DIR_R &= 0b11110000;

    // 3) Enable digital functionality for button inputs,
    //    do not modify other PORTE enables
    GPIO_PORTE_DEN_R |= 0b00001111;
	
	initialized = 1;
}



/**
 * Initialize and configure PORTE interupts
 */
void init_button_interrupts() {

    // 1) Mask the bits for pins 0-3
    GPIO_PORTE_IM_R &= 0b11110000;

    // 2) Set pins 0-3 to use edge sensing
    GPIO_PORTE_IS_R &= 0b11110000;

    // 3) Set pins 0-3 to use both edges. We want to update the LCD
    //    when a button is pressed, and when the button is released.
    GPIO_PORTE_IBE_R |= 0b00001111;

    // 4) Clear the interrupts
    GPIO_PORTE_ICR_R = 0xFF;

    // 5) Unmask the bits for pins 0-3
    GPIO_PORTE_IM_R |= 0b00001111;

    // 6) Enable GPIO port E interrupt
    NVIC_EN0_R |= 0x00000010;

    // Bind the interrupt to the handler.
    IntRegister(INT_GPIOE, gpioe_handler);
}


/**
 * Interrupt handler -- executes when a GPIO PortE hardware event occurs (i.e., for this lab a button is pressed)
 */
void gpioe_handler() {

    // Clear interrupt status register
    GPIO_PORTE_ICR_R = 0xFF;
    button_event = 1;
    button_num = button_getButton();
}




/**
 * Returns the position of the rightmost button being pushed.
 * @return the position of the rightmost button being pushed. 4 is the rightmost button, 1 is the leftmost button.  0 indicates no button being pressed
 */
uint8_t button_getButton() {

    if (button_event){
        if ((GPIO_PORTE_DATA_R & 0b00001000) == 0b00000000){
            return 4;
        } else if ((GPIO_PORTE_DATA_R & 0b00000100) == 0b00000000){
            return 3;
        } else if ((GPIO_PORTE_DATA_R & 0b00000010) == 0b00000000){
            return 2;
        } else if ((GPIO_PORTE_DATA_R & 0b00000001) == 0b00000000){
            return 1;
        }

    }

	return 0;
}





