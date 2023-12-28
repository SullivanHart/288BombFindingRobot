/*
 * adcMic Header
 *
 * 12\2\2023
 * @author Daniel, Laith, Logan, Sullivan
 */

#ifndef ADCMIC_H_
#define ADCMIC_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

void adcMic_init(void);

int adcMic_read(void);

int adcMic_scan(void);

#endif /* ADCMIC_H_ */
