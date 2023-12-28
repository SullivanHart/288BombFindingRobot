/*
 * ping Header
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */


#ifndef PING_H_
#define PING_H_

// Initialize the ping sensor
void ping_init(void);

// Get distance from PING sensor
int ping_read(void);

// send out a pulse on PB3
void send_pulse(void);

// ISR captures PING sensor's response pulse start and end time
void TIMER3B_Handler(void);

// Check for an overflow, if there is, it corrects it, otherwise, returns the count (Difference)
int overflowCorrector(int rising_time, int falling_time);

#endif
