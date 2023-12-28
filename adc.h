/*
 * ADC header
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

#ifndef ADC_H_
#define ADC_H_

// initialize the port and the ADC module
void adc_init(void);

// returns the raw conversion value
int adc_read(void);

#endif
