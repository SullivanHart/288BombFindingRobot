/*
 * ADC
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

#include "Timer.h"
#include "adc.h"

// initialize the port and the ADC module
void adc_init(void){

    // GPOI Port B (pin 4) initialization
    SYSCTL_RCGCGPIO_R |= 0x02;          // enable clock GPIOB
    timer_waitMillis(100);              // Delay

    GPIO_PORTB_AFSEL_R |= 0x10;          // sets PB4 as a peripheral
    GPIO_PORTB_AMSEL_R |= 0x10;          // Enable analog function for pin 4
    GPIO_PORTB_PCTL_R  &= 0xFFF0FFFF;    // No digital function is needed
    GPIO_PORTB_DEN_R   &= 0xEF;          // Clear PortB pin 4
    GPIO_PORTB_DIR_R   &= 0xEF;          // sets pb4 as input
    GPIO_PORTB_ADCCTL_R &= 0xEF;          // No external trigger

    // ADC Module 0 initialization
    SYSCTL_RCGCADC_R |= 0x1;    // Enable clock ADC0
    timer_waitMillis(100);      // Delay

    ADC0_ACTSS_R &= 0x7;        // Disable ADC MUXSS3
    ADC0_SSMUX3_R = 0x0A;       // SS3 (AIN10)
    ADC0_SSCTL3_R = 0x06;       // End of sequence 0b0110
    ADC0_EMUX_R &= 0x0FFF;      // triggered by setting the bit in PSSI
//  ADC0_SAC_R = 0x4;           // 16 samples average
    ADC0_ACTSS_R |= 0x8;        // Enable ADC MUXSS3
}

// returns the raw conversion value
int adc_read(void){

    int i;
    int result = 0;

    for (i = 0; i < 16; i++){

        ADC0_PSSI_R |= 0x8; // trigger

        while ((ADC0_SSFSTAT3_R & 0x1000) == 0){  // wait for a new value

        }

        ADC0_ISC_R |= 0x8; // Clear interrupt

        result += ADC0_SSFIFO3_R; // Copy result
    }


    return (result / 16);
}
