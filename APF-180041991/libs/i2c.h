#ifndef I2C_H
#define I2C_H

#include <msp430.h>
#include <stdint.h>
#include "gpio.h"

void i2cInit();
uint8_t i2cSend(uint8_t addr, uint8_t * data, uint8_t nBytes);
uint8_t i2cSendByte(uint8_t addr, uint8_t byte);
uint8_t i2cGet(uint8_t addr, uint8_t * data, uint8_t nBytes);
uint8_t i2cGetByte(uint8_t addr, uint8_t * byte);

#endif /* I2C_H */
