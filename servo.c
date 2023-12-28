/*
 * Servo Funtions (PWM)
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */
#include <stdio.h>
#include "Timer.h"
#include "servo.h"
#include "button.h"
#include "lcd.h"

extern volatile int button_event;
extern volatile int button_num;

int right_calib = 16000;
int left_calib = 16000;

//Initializing the servo
void servo_init(void){
    int period = 320000;
    int width = 320000 - 16000;
    // GPIO Initialization
    SYSCTL_RCGCGPIO_R |= 0x02;          // Enable clock GPIO Port B
    timer_waitMillis(100);              // Delay
    GPIO_PORTB_AFSEL_R |= 0x20;         // sets PB3 as a peripheral
    GPIO_PORTB_PCTL_R  &= 0xFF0FFFFF;   // Clear the bits for PB5 PCTL
    GPIO_PORTB_PCTL_R  |= 0x00700000;   // Select (T1CCP1)
    GPIO_PORTB_DEN_R   |= 0x20;         // Digital Enable

    // Timer Initialization
    SYSCTL_RCGCTIMER_R |= 0x02; // Enable clock Timer 1
    timer_waitMillis(100);      // Delay
    TIMER1_CTL_R &= ~0x0100;    // Disable TimerB
    TIMER1_CFG_R = 0x4;         // 16 bit timer
    TIMER1_TBMR_R |= 0x000A;    // Periodic mode, PWM Enable
    TIMER1_TBMR_R &= ~0x0010;   // Count Down
    TIMER1_TBILR_R = (period & 0xFFFF);    // Start Value (16 Bits) Interval Load
    TIMER1_TBPR_R = (period >> 16);   // Start Value (8 Bits) Prescale
    TIMER1_TBMATCHR_R = (width & 0xFFFF);
    TIMER1_TBPMR_R = (width >> 16);
    TIMER1_CTL_R |= 0x0100;     // Enable TimerB
}

// Move the servo to the degree specified, the servo should be initialized before using this function
int servo_move(float degree){
    int width = ((right_calib - left_calib) / 180) * (degree) + left_calib;
    TIMER1_TBMATCHR_R = ((320000 - width) & 0xFFFF);
    TIMER1_TBPMR_R = ((320000 - width) >> 16);
    return 0;
}

// Set the calibration values, the servo should be initialized before using this function
void set_calib_values(int right, int left){
    right_calib = right;
    left_calib = left;
    servo_move(90);
}

// Calibrating the servo using the lcd buttons, the servo and lcd should be initialized before using this function
void servo_calib(){
    int direction = 1;
    int width = 24000;
    char print_str[40];
    char dir_str[20];

    while (1){

        if(button_event)
        {
            if (button_num == 1)
            {
                width = width + (direction * 100);

            }
            else if (button_num == 2)
            {
                width = width + (direction * 500);
            }
            else if (button_num == 3)
            {
                if (direction == 1){
                    direction = -1;
                    sprintf(dir_str,"%s", "clockwise");
                }
                else
                {
                    direction = 1;
                    sprintf(dir_str,"%s","counter clockwise");
                }
            }
            else if (button_num == 4)
            {
                if (direction == 1){
                    width = 32000;
                } else {
                    width = 16000;
                }
            }

            TIMER1_TBMATCHR_R = ((320000 - width) & 0xFFFF);
            TIMER1_TBPMR_R = ((320000 - width) >> 16);
            button_event = 0;
        }

        sprintf(print_str,"%d\n%s", width, dir_str);
        lcd_printf(print_str);
    }
}
