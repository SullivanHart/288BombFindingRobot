/*
 * ADC for Microphone
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

#include "adcMic.h"
#include "timer.h"

/*
 * TO ATTATCH TO BOT:
 * On Launchpad, use jumper pins.
 * Connect wires from mic to pins
 * -OUT to PD0
 * -VCC to +3.3V
 * -GND to GND
 */

void adcMic_init(void)
{
    //Port D, pin 0 (AIN 7)
    SYSCTL_RCGCGPIO_R |= 0b001000;
    GPIO_PORTD_AFSEL_R |= 0b00000001;
    GPIO_PORTD_DEN_R |= 0b00000001; 
    GPIO_PORTD_DIR_R &= 0b11111110; 
    GPIO_PORTD_AMSEL_R |= 0b00000001;

    //ADC 1
    SYSCTL_RCGCADC_R |= 0b10;
    timer_waitMillis(1);
    ADC1_ACTSS_R |= 0b00001;

    ADC1_SSMUX0_R |= 7;
    ADC1_SSCTL0_R |= 0b0110;
}

int adcMic_read(void)
{
    ADC1_PSSI_R |= 0b000001;
    timer_waitMillis(5);
    while(~ADC1_RIS_R & 0x01)
    {
    }
    ADC1_ISC_R |= 0x01;
    return ADC1_SSFIFO0_R;
}

int adcMic_scan(void)
{
    int trials  = 25;  //number of times to find amplitude
    int readings = 5; //number of times to read from mic each time
    int i = 0;
    int x = 0;
    int result = 0;
    int average = 0;
    int soundMax = 0;
    int soundMin = 5000;    //theoretical max mV from microphone

    //find amplitude
    for (; i < trials; i++){
        result = 0;
        x = 0;
        for (; x < readings; x++){
            //read from mic
            result += adcMic_read();
        }
        average = result / readings;
        //only store highest & lowest
        if ((average < 5000) && (average > 0))  //ensure readings are within expected range (no static)
            if (average > soundMax){
                soundMax = average;
            } else if (average < soundMin){
                soundMin = average;
            }
        }
    int amp = (soundMax - soundMin) / trials;
    return amp;
}

