#ifndef TIMER_H
#define TIMER_H

#include <msp430.h>
#include <stdint.h>

typedef enum {us, ms, sec} timeUnit_t;

void wait(uint16_t time, timeUnit_t unit);

#endif /* TIMER_H */
