#include "i2c.h"
 /* sda no 4.1
    scl no 4.2 */

void i2cInit()
{
    /*
    1) Setar o bit de RST
    2) Inicializar os registros de config
    3) Configurar pinos
    4) Limpar o RST
    5) Habilitar Interrupções
    */
    // ucb0 MESTRE 100kHz addr 0x12
    UCB1CTL1 = UCSWRST;
    UCB1CTL0 = UCMST | UCMODE_3 | UCSYNC;
    UCB1CTL1 |= UCSSEL__SMCLK | UCTR;
    UCB1BRW = 100;

    /* configurando endereço */
//    UCB0I2COA |= 0x12;

    /* configurando pinos */
    pinMode(4, 1, input, PSEL_1);
    pinMode(4, 2, input, PSEL_1);

    /* liga os resistores dependendo da situação */
//    pinMode(4, 1, inPullUp, PSEL_1);
//    pinMode(4, 2, inPullUp, PSEL_1);

    /* Limpar o RST */
    UCB1CTL1 &= ~UCSWRST;

    /* Ativar interrupções */
//    UCB1IE = UCRXIE | UCTXIE | UCNACKIE;
//    __enable_interrupt();
}

uint8_t i2cSend(uint8_t addr, uint8_t * data, uint8_t nBytes)
{
    UCB1IFG &= ~UCTXIFG;                    // Clear flag
    UCB1I2CSA = addr;                       // Endereço do escravo
    UCB1CTL1 |= UCTR;                       // Transmitter
    UCB1CTL1 |= UCTXSTT;                    // Transmite o start

    while(!(UCB1IFG & UCTXIFG));            // Espera o start acontecer
    UCB1TXBUF = *data++;                    // Primeiro byte para destravar
    nBytes--;                               // o ciclo de ACK

    while(UCB1CTL1 & UCTXSTT);              // Aguarda ACK/NACK
    //while(!(UCB1IFG & UCTXIFG) && !(UCB1IFG & UCNACKIFG)); // Mesma coisa da linha de cima

    if(UCB1IFG & UCNACKIFG)
    {                                       // Se foi NACK
        UCB1CTL1 |= UCTXSTP;                // pede stop
        while(UCB1CTL1 & UCTXSTP);          // espera
        return 1;                           // retorna 1
    }

    while(nBytes--)                         // Se foi ACK
    {                                       // continua enviando
        while(!(UCB1IFG & UCTXIFG));        // os próximos bytes
        UCB1TXBUF = *data++;                // toda vez que TXBUF ficar vazio
    }

    while(!(UCB1IFG & UCTXIFG));            // Espera enviar o último byte

    UCB1CTL1 |= UCTXSTP;                    // Pede stop
    while(UCB1CTL1 & UCTXSTP);              // Espera o stop
    UCB1IFG &= ~UCTXIFG;                    // Clear flag

    return 0;                               // 0 = sucesso
}

uint8_t i2cSendByte(uint8_t addr, uint8_t byte)
{
    return i2cSend(addr, &byte, 1);
}

uint8_t i2cGet(uint8_t addr, uint8_t * data, uint8_t nBytes)
{
    UCB1IFG &= ~UCRXIFG;                    // Clear flag
    UCB1I2CSA = addr;                       // Endereço do escravo
    UCB1CTL1 &= ~UCTR;                      // Receiver
    UCB1CTL1 |= UCTXSTT;                    // Transmite o start

    while(UCB1CTL1 & UCTXSTT);              // Aguarda ACK/NACK

    if(UCB1IFG & UCNACKIFG)
    {                                       // Se foi NACK
        UCB1CTL1 |= UCTXSTP;                // pede stop
        while(UCB1CTL1 & UCTXSTP);          // espera
        return 1;                           // retorna 1
    }

    while(nBytes--)
    {
        if(nBytes == 0)
        {
            UCB1CTL1 |= UCTXSTP;            // Pede stop antes do último byte
        }
        while(!(UCB1IFG & UCRXIFG));        // Espera um byte
        *data++ = UCB1RXBUF;                // salva o byte
    }

    while(UCB1CTL1 & UCTXSTP);              // Espera o stop
    UCB1IFG &= ~UCRXIFG;                    // Clear flag

    return 0;
}

uint8_t i2cGetByte(uint8_t addr, uint8_t * byte)
{
    return i2cGet(addr, byte, 1);
}
