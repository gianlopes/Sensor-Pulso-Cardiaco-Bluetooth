#ifndef ADC_H
#define ADC_H


#include <msp430.h>
#include <stdint.h>
#include "gpio.h"

void adcInit();

/* Variáveis globais para comunicar com a main */
extern uint16_t intervals[8];

#endif /* ADC_H */
