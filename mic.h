/*
*   mic.h
*/

#ifndef MIC_H_
#define MIC_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"


extern volatile  int mic_data;
extern volatile  char micFlag;


void mic_init();

int mic_receive();

void mic_interrupt_init();

void mic_interrupt_handler();

#endif /* MIC_H_ */
