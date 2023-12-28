/*
 * servo Header
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

#ifndef SERVO_H_
#define SERVO_H_

//Initializing the servo
void servo_init(void);

// Move the servo to the degree specified, the servo should be initialized before using this function
int servo_move(float degree);

// Set the calibration values, the servo should be initialized before using this function
void servo_calib(void);

// Calibrating the servo using the lcd buttons, the servo and lcd should be initialized before using this function
void set_calib_values(int right, int left);

#endif
