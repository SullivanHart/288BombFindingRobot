/*
 * Scans using the servo and ir
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

#include "ping.h"
#include "servo.h"
#include "adc.h"
#include "Timer.h"

typedef struct {
    int pingData;
    int irData;
} ScanData;

void search(ScanData *scanData, int ang){
    servo_move(ang);

    float ir = 0;
    ir += adc_read();
    timer_waitMillis(100);
    ir += adc_read();
    timer_waitMillis(100);
    ir += adc_read();
    timer_waitMillis(100);

    ir /= 3;
    scanData->irData = ir;
    timer_waitMillis(100);

    scanData->pingData = ping_read();
}
