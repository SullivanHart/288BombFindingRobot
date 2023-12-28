/*
 * Main for bomb-detecting robot
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

// Includes
#include <stdbool.h>
#include <stdint.h>
#include <inc/tm4c123gh6pm.h>
#include <Math.h>

#include "Timer.h"
#include "lcd.h"
#include "uart.h"
#include "driverlib/interrupt.h"
#include "open_interface.h"
#include "movement.h"
#include "adc.h"
#include "ping.h"
#include "servo.h"
#include "adcMic.h"

// Prototypes
void fullServoScan();
void print_str_putty(char str[]);
void distance_calculation();
char get_average(char arr[], int start, int end);
void autoDrive(oi_t *sensor_data);
void printAngleValues();
void printObjectStats();
void servoScan(char str[]);
int fastServoScan();

// ---------------------------

// Global Variables
volatile char uart_data;  // Your UART interrupt code can place read data here
volatile char flag;       // Your UART interrupt can update this flag

// Arrays
char distance_ping[91];
char distance_IR[91];
char degrees[91];
char objAngle[10];
char objDistance[10];
char objWidth[10];
char objLinearWidth[10];
char objCount = 0;

//Hardcoded values
float vol = 43.0;   //Volume of bomb

int main(void){

    // Initialization
    timer_init();
    lcd_init();
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    uart_init();
    uart_interrupt_init();
    adc_init();
    ping_init();
    servo_init();
    set_calib_values(35900, 7500);
    adcMic_init();

    while(1){

        //FLAG CONDITION CHECK
        if (flag){
            flag = 0;

            if(uart_data == 'm'){

                // Scan 180 Degrees
                fullServoScan();

                // Do the calculation (after servo_scan_180)
                distance_calculation();

                // Print angle values
                printAngleValues();

                // Objects stats printing
                printObjectStats();
            } else if (uart_data == 'p'){
                fullServoScan(); //SCAN

                // Do the calculation (after servo_scan_180)
                distance_calculation();

                // Print angle values
                printAngleValues();

                // Objects stats printing
                printObjectStats();

                // Drive
                autoDrive(sensor_data);
            } else if (uart_data == 'n'){
               char str[20];
               servoScan(str);
               lcd_printf("%s",str);
               print_str_putty(str);
            } else if (uart_data == 'g'){
                printObjectStats();
            } else if (uart_data == 'w') {
               if (fastServoScan()){
                   char str[] = "Object!";
                   uart_sendStr(str);
               } else {
                   move_forward(sensor_data, 5, 150, 1);
                   lcd_printf("Left:%d Right:%d\nLeft:%d Right:%d", sensor_data->cliffFrontLeftSignal, sensor_data->cliffFrontRightSignal, sensor_data->cliffLeftSignal, sensor_data->cliffRightSignal);
               }

            }
            else if (uart_data == 'd') {
               uart_sendStr("Turned 5 degrees right");
               turn_clockwise(sensor_data, 5);

            }
            else if (uart_data == 'a') {
                uart_sendStr("Turned 5 degrees left");
                turn_counterclockwise(sensor_data, 5);
            }
            else if (uart_data == 's') {
                move_backward(sensor_data, 5, 150, 1);
            }
            else if (uart_data == 'f') {
                oi_free(sensor_data);
                char str[] = "free";
                uart_sendChar('\n');
                uart_sendChar('\r');
                print_str_putty(str);
            }
            else if (uart_data == 'q') {
                oi_update(sensor_data);
                lcd_printf("Left:%d Right:%d\nLeft:%d Right:%d", sensor_data->cliffFrontLeftSignal, sensor_data->cliffFrontRightSignal, sensor_data->cliffLeftSignal, sensor_data->cliffRightSignal);
            }
            else if (uart_data == 'r') {
                //Scan sound in front of bot
                servo_move(0);
                timer_waitMillis(250);
                float right = adcMic_scan();
                timer_waitMillis(500);
                servo_move(90);
                timer_waitMillis(250);
                float mid = adcMic_scan();
                timer_waitMillis(500);
                servo_move(180);
                timer_waitMillis(250);
                float left = adcMic_scan();
                timer_waitMillis(500);
                servo_move(90);

                //convert to percentage
                left /= vol;
                mid /= vol;
                right /= vol;

                //Print reading
                char str[100];
                sprintf(str,"L: %.02f%%\tM: %.02f%%\tR: %.02f%%\n\r", left * 100, mid * 100, right * 100);
                print_str_putty(str);
            } else if (uart_data == 't') {
                //read in front of bot
                float amp = adcMic_scan();
                timer_waitMillis(500);

                //Print reading
                char str[100];
                sprintf(str,"Volume: %.02f%%\tAmp: %d\n\r", ((amp / vol) * 100), (int)amp);
                print_str_putty(str);
            } else if (uart_data == 'b') {
                int song = 1;
                int songNumNotes = 16;
                unsigned char songNotes[16] = {55, 55, 55, 51, 58, 55, 51, 58, 55, 0,  62, 62, 62, 63, 58, 54};
                unsigned char songTimes[16] = {32, 32, 32, 20, 12, 32, 20, 12, 32, 32, 32, 32, 32, 20, 12, 32};
                oi_loadSong(song, songNumNotes, songNotes, songTimes);
                oi_play_song(song);
            }
        }
    }
}

// Takes a char pointer and prints till null
void print_str_putty(char str[]){
    int strindex = 0;

    while (str[strindex] != 0){
        uart_sendChar(str[strindex]);
        strindex++;
    }
}

// Servo Scan function
void fullServoScan(){

    int i;
    int j;
    int sound_distance_sum = 0;
    int IR_distance_sum = 0;
    servo_move(0);
    timer_waitMillis(250);

    for (i = 0; i < 181; i+=2){
        // take tree different samples at the same angle
        for (j = 0; j < 3; j++){
            servo_move(i);
            timer_waitMillis(50);
            sound_distance_sum += (int) ping_read();
            IR_distance_sum += (int) 50000000 * pow(adc_read() , -1.98); // Bot #15
        }

        sound_distance_sum = sound_distance_sum / 3;
        IR_distance_sum = IR_distance_sum / 3;

        degrees[(i / 2)] = i;
        distance_ping[(i / 2)] = sound_distance_sum;
        distance_IR[(i / 2)] = IR_distance_sum; // cyBot 15
        lcd_printf("%d, %d, %d", i, distance_ping[(i / 2)], distance_IR[(i / 2)]);

        sound_distance_sum = 0;
        IR_distance_sum = 0;
    }
}

int fastServoScan(){
    //Check for objects before each move forward
    int i;
    int j;
    int count = 0;
    int sound_distance_sum = 0;

    servo_move(45);
    timer_waitMillis(250);

    for (i = 30; i < 150; i+=2){
        // take tree different samples at the same angle
        for (j = 0; j < 3; j++){
            servo_move(i);
            timer_waitMillis(1);
            sound_distance_sum += (int) ping_read();
        }

        sound_distance_sum = sound_distance_sum / 3;

        if (sound_distance_sum <= 20){
            count++;
        } else {
            count = 0;
        }

        if (count >= 4){
            return 1;
        }

        sound_distance_sum = 0;
    }

    return 0;
}

// Servo Scan function
void servoScan(char str[]){

    int j;
    int sound_distance_sum = 0;
    int IR_distance_sum = 0;
    servo_move(90);
    timer_waitMillis(250);

    // take three different samples at the same angle
    for (j = 0; j < 3; j++){
        sound_distance_sum += (int) ping_read();
        IR_distance_sum += (int) 50000000 * pow(adc_read() , -1.98); // Bot #15
        timer_waitMillis(50);
    }

    sprintf(str,"90, %d, %d\n\r", (sound_distance_sum / 3), (IR_distance_sum / 3));
}

void distance_calculation(){

    int i;
    int start_angle = 0;
    int value = distance_IR[0];
    int count = 0;
    objCount = 0;

    for (i = 1; i < 91; i++) // compare distance_IR[i] to distance[i-1]
    {
        // compare to the first value
        if ((count < 3) && abs(distance_IR[i] - value) <= 3)
        {
            count++;
        }
        else if ((count >= 3 && count <= 5) && abs(distance_IR[i] - value) <= 4)
        {
            count++;
        }
        else if ((count > 5) && abs(distance_IR[i] - value) <= 6)
        {
            count++;
        }
        else if ((count >= 3) && (distance_IR[i - count - 1] <= 50) && (distance_IR[i - count - 1] >= 10))
        {
            int mid_i = i - ((i - start_angle) / 2);
            // In degrees
            objWidth[objCount] = count * 2;
            // Distance using the ping (Sound Sensor)
            objDistance[objCount] = distance_ping[mid_i];
            // The object's angular location
            objAngle[objCount] = degrees[mid_i];
            //  for next comparison
            value = distance_IR[i];
            objCount++;
            count = 0;
        }
        else
        {
            value = distance_IR[i];
            start_angle = i;
            count = 0;
        }
    }

    // calculate the linear width
    int j;
    double radians;

    for (j = 0; j < objCount; j++)
    {
        radians = ((double) (objWidth[j])) * (3.14159265 / 180.0);
        objLinearWidth[j] = 2 * (int) objDistance[j] * tan((radians / 2.0));
    }
}

void autoDrive(oi_t *sensor_data){

    int smallest_obj = 0;
    int turn_degrees;
    int i;

    for (i = 1; i < objCount; i++)
    {
        if (objLinearWidth[i] < objLinearWidth[smallest_obj]){
            smallest_obj = i;
        }
    }

    turn_degrees = (objAngle[smallest_obj] - 90);

    if (turn_degrees > 0){
        turn_counterclockwise(sensor_data, turn_degrees);
        move_forward(sensor_data, objDistance[smallest_obj] , 100, 1);
    } else if (turn_degrees < 0){
        turn_clockwise(sensor_data, abs(turn_degrees));
        move_forward(sensor_data, (objDistance[smallest_obj] - 20) , 100, 1);
    }

}

void printAngleValues(){
    char uartArr[50];

    // Print the distances values at each degree from the both sensors
    int j;

    for (j = 0; j < 91; j++)
    {
        sprintf(uartArr, "%d\t%d", (j) *2, distance_IR[j]);
        uart_sendStr(uartArr);
    }
    uart_sendStr("END\n");
}

void printObjectStats(){
    char arr[50];
    sprintf(arr,"Object#  Angle   Distance  Width (Linear)width\n");
    print_str_putty(arr);

    char str[10];
    int j;

    for (j = 0; j < objCount; j++) {

        sprintf(str, "%d", (j + 1));
        print_str_putty(str);

        sprintf(str, "\t%d", objAngle[j]);
        print_str_putty(str);

        sprintf(str, "\t%d", objDistance[j]);
        print_str_putty(str);

        sprintf(str, "\t%d", objWidth[j]);
        print_str_putty(str);

        sprintf(str, "\t%d", objLinearWidth[j]);
        print_str_putty(str);

        uart_sendChar('\n');
    }

    sprintf(arr,"END\n");
    print_str_putty(arr);
}
