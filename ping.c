/*
 * Ping Sensor
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

// Includes
#include "Timer.h"
#include "ping.h"

// Variables
volatile enum {LOW, HIGH, DONE} state;  // Set by ISR - Reading State
volatile unsigned int rising_time;      //Pulse start time: Set by ISR
volatile unsigned int falling_time;     //Pulse end time: Set by ISR

// Initialize the ping sensor
void ping_init(void){

    // GPIO Initialization
    SYSCTL_RCGCGPIO_R |= 0x02;          // Enable clock GPIOB
    timer_waitMillis(100);              // Delay
    GPIO_PORTB_PCTL_R  &= 0xFFFF0FFF;   // Clear the bits for PB3 PCTL
    GPIO_PORTB_PCTL_R  |= 0x00007000;   // Select (T3CCP1)
    GPIO_PORTB_DEN_R   |= 0x08;         // Digital Enable

    // Timer Initialization
    SYSCTL_RCGCTIMER_R |= 0x08; // Enable clock Timer3
    timer_waitMillis(100);      // Delay
    TIMER3_CTL_R &= ~0x0100;    // Disable TimerB
    TIMER3_IMR_R &= ~0x0400;    // Disable GPTM Timer B Capture Mode Event Interrupt

    // Configuration
    TIMER3_CFG_R = 0x4;         // 16 Bit timer
    TIMER3_TBMR_R |= 0x0007;    // Capture Mode, and Edge Time Mode
    TIMER3_TBMR_R &= ~0x0010;   // Counts down
    TIMER3_CTL_R |= 0x0C00;     // Both Edges
    TIMER3_TBILR_R = 0xFFFF;    // Start Value (16 Bits) Interval Load
    TIMER3_TBPR_R = 0xFF;       // Start Value (8 Bits) Prescale
    TIMER3_ICR_R |= 0x0400;     // Clear Interrupts
    TIMER3_IMR_R |= 0x0400;     // Enable GPTM Timer B Capture Mode Event Interrupt
    TIMER3_CTL_R |= 0x0100;     // Enable TimerB

    NVIC_EN1_R |= 0x00000010;   // Enable NVIC 36

    // Bind the interrupt
    IntRegister(INT_TIMER3B, TIMER3B_Handler);
}

// ISR captures PING sensor's response pulse start and end time
void TIMER3B_Handler(void){

    if (TIMER3_MIS_R & 0x400)
    {
        if (state == HIGH) // Rising edge
        {
            rising_time = (TIMER3_TBR_R & 0xFFFFFF);
            state = LOW;
        }
        else if (state == LOW) // Falling edge
        {
            falling_time = (TIMER3_TBR_R & 0xFFFFFF);
            state = DONE;
        }

        // Clear the interrupt
        TIMER3_ICR_R |= 0x0400;
    }
}

// Get distance from PING sensor
int ping_read(void){

    // Set initial state
    state = HIGH;

    // Send a trigger pulse to the sensor
    send_pulse();

    // Wait for ISR to capture rising edge & falling edge time
    while (state != DONE){}

    // Calculate the width of the pulse; convert to centimeters
    int count = overflowCorrector(rising_time, falling_time);
    double time_sec =  count * (1.0 / 16000000.0);
    int velocity_sound_cm = (34000.0 / 2.0);
    int distance = time_sec * velocity_sound_cm;

    return distance;
}

// send out a pulse on PB3
void send_pulse(void){
    TIMER3_CTL_R &= ~0x100; // Disable the counter
    TIMER3_IMR_R &= ~0x400; // Mask the interrupt

    GPIO_PORTB_AFSEL_R &= 0xF7; // Get signal from DATA R
    GPIO_PORTB_DIR_R |= 0x08;   // Set PB3 as output
    GPIO_PORTB_DATA_R |= 0x08;  // Set PB3 to high
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= 0xF7;  // Set PB3 to low
    GPIO_PORTB_DIR_R &= 0xF7;   // Set PB3 as input
    GPIO_PORTB_AFSEL_R |= 0x08; // sets PB3 as a peripheral

    TIMER3_IMR_R |= 0x400;  // Mask the interrupt
    TIMER3_ICR_R |= 0x400;  // Clear the interrupt
    TIMER3_CTL_R |= 0x100;  // Enable the counter
}

// Check for an overflow, if there is, it corrects it, otherwise, returns the count (Difference)
int overflowCorrector(int rising_time, int falling_time){

    // Result of the calculation
    int result = 0;
    
    if (falling_time > rising_time) // falling time is greater than Rising time
    {
       result = rising_time + (0xFFFFFF - falling_time);
    }
    else // Rising time is greater than Falling time
    {
        result = rising_time - falling_time;
    }

    return result; 
}
