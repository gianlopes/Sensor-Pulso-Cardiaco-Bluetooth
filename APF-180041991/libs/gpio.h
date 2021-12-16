#ifndef GPIO_H
#define GPIO_H

#include <msp430.h>
#include <stdint.h>

typedef enum {
    reset=0,set=1,toggle=2
} operation_t;

typedef enum {
    input, output, inPullUp, inPullDown
} pinModes_t;

typedef enum {
    offset_in=0x0,
    offset_out=0x2,
    offset_dir=0x4,
    offset_ren=0x6,
    offset_sel=0xA
} pinOffset_t;

typedef enum {
    PSEL_0, PSEL_1
} pinSel_t;

/* altera port.bit segundo a operação fornecida */
void digitalWrite(uint8_t port, uint8_t bit, operation_t operation);

/* retorna 0, se o pino estiver em LOW
 * retorna qualquer valor diferente de zero se o pino estiver em HIGH
 */
uint8_t digitalRead(uint8_t port, uint8_t bit);

/* Configura um pino */
void pinMode(uint8_t port, uint8_t bit, pinModes_t mode, pinSel_t pinSel);

/* Função auxiliar */
uint8_t * getAdress(uint8_t port, pinOffset_t offset);

#endif /* GPIO_H */
