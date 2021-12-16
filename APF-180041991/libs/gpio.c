#include "gpio.h"

// altera port.bit
// opções:
// 0 - reset
// 1 - set
// 2 - toggle
void digitalWrite(uint8_t port, uint8_t bit, operation_t op)
{
    uint8_t mask = 1 << bit;

    uint8_t * POUT = getAdress(port,offset_out);

    switch (op) {
        case reset:
            *POUT &= ~mask;
            break;
        case set:
            *POUT |= mask;
            break;
        case toggle:
            *POUT ^= mask;
            break;
        default:
            break;
    }
    return;
}

// retorna 0, se o pino estiver em LOW
// retorna qualquer valor diferente de zero se o pino estiver em HIGH
uint8_t digitalRead(uint8_t port, uint8_t bit)
{
    uint8_t mask = 1 << bit;

    uint8_t * PIN = getAdress(port,offset_in);
    return *PIN & mask;
}

// Configura um pino
void pinMode(uint8_t port, uint8_t bit, pinModes_t mode, pinSel_t pinSel)
{
    uint8_t mask = 1 << bit;

    uint8_t *PDIR, *PREN, *POUT, *PSEL;
    PDIR = getAdress(port,offset_dir);
    PREN = getAdress(port,offset_ren);
    POUT = getAdress(port,offset_out);
    PSEL = getAdress(port,offset_sel);

    switch (mode) {
        case output:
            *PDIR |= mask;
            break;
        case input:
            *PDIR &= ~mask;
            *PREN &= ~mask; // sem resistor
            *POUT &= ~mask;
            break;
        case inPullUp:
            *PDIR &= ~mask;
            *PREN |= mask; // com resistor
            *POUT |= mask; // pull up
            break;
        case inPullDown:
            *PDIR &= ~mask;
            *PREN |= mask; // com resistor
            *POUT &= ~mask; // pull down
            break;
        default:
            break;
    }

    switch (pinSel) {
        case PSEL_0:
            *PSEL &= ~mask;
            break;
        case PSEL_1:
            *PSEL |=  mask;
            break;
        default:
            *PSEL &= ~mask;
            break;
    }

    return;
}

uint8_t * getAdress(uint8_t port, pinOffset_t offset)
{
    uint8_t * adress =
            (uint8_t *) (
                        // Endereço base
                        0x200
                        // Endereço das portas (0 para 1,2 e 20 para 3,4, etc...)
                        + ( (port - 1) >> 1 ) * 0x20 // igual a ( (port - 1) / 2 ) * 0x20
                        // Endereço de Px (x impar = 0, x par = 1)
                        + ( (port + 1) & BIT0) // igual a ( (port + 1) % 2)
                        // Qual tipo (in,out,dir,ren)
                        + offset
                        );
    return adress;
}
