#include "uart.h"

void uartInit()
{
    /*
     * UART usada na comunicação com o PC
     * Configuração da USCI-A1
     * Convenção usada:
     * baudrate = 19200
     * LSB-first
     * sem paridade
     * stop bits = 1
     */
    UCA1CTL1 = UCSWRST;                     // Inicia a interface resetada
    UCA1CTL0 = 0;                           // Usa a convenção padrão
    UCA1CTL1 |= UCSSEL__SMCLK;              // D = 1048576 / 19200 / 16 = 3.41
    UCA1BRW = 3;                            // BRW = 3
    UCA1MCTL = UCOS16 | UCBRF_7;            // BRF = 0.41 * 16 => 7
    P4SEL |= BIT4 | BIT5;                   // Pinos P4.4 e P4.5

    UCA1CTL1 &= ~UCSWRST;                   // Libera o rst
    UCA1IE = UCRXIE;                        // Habilita a interrupção de RX

    /*
     * UART usada no BlueTooth
     * Configuração da USCI-A0
     * Convenção usada:
     * baudrate = 9600
     * LSB-first
     * sem paridade
     * stop bits = 1
     */
    UCA0CTL1 = UCSWRST;                     // Inicia a interface resetada
    UCA0CTL0 = 0;                           // Usa a convenção padrão
    UCA0CTL1 |= UCSSEL__SMCLK;              // D = 1048576 / 9600 / 16 = 6.82
    UCA0BRW = 6;                            // BRW = 6
    UCA0MCTL = UCOS16 | UCBRF_13;           // BRF = 0.82 * 16 => 13
    P3SEL |= BIT3 | BIT4;                   // Pinos P3.3 e P3.4

    UCA0CTL1 &= ~UCSWRST;                   // Libera o rst
//    UCA0IE = UCRXIE;                        // Habilita a interrupção de RX
}

void uartSend(uint8_t byte, uartInterface uartXX)
{
    if(uartXX == uartPC){
        while(!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = byte;
    }
    if(uartXX == uartBT)
    {
        while(!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = byte;
    }
}

/* Expects \0 terminated string */
void uartPrint(char * str, uartInterface uartXX)
{
    while(*str)
        uartSend(*str++, uartXX);
}

void uartPrintChar(char c, uartInterface uartXX)
{
    uartSend(c, uartXX);
}

void uartDec8(uint8_t num, uartInterface uartXX)
{
    uint8_t c, d, u, r;

    c = num / 100;
    r = num % 100;

    d = r / 10;
    r = r % 10;

    u = r;

    uartPrintChar(c + 0x30, uartXX);
    uartPrintChar(d + 0x30, uartXX);
    uartPrintChar(u + 0x30, uartXX);
}

void uartDec16(uint16_t num, uartInterface uartXX)
{
    uint16_t dm, m, c, d, u, r;

    dm = num / 10000;
    r = num % 10000;

    m = num / 1000;
    r = num % 1000;

    c = r / 100;
    r = r % 100;

    d = r / 10;
    r = r % 10;

    u = r;

    if(dm > 0)
        uartPrintChar(dm+ 0x30, uartXX);
    if(m > 0)
        uartPrintChar(m + 0x30, uartXX);
    uartPrintChar(c + 0x30, uartXX);
    uartPrintChar(d + 0x30, uartXX);
    uartPrintChar(u + 0x30, uartXX);
}

/* Usada para enviar os dados em forma de bytes.
 * As outras funções enviam em formato ascii,
 * porém o código usado para plotar o gráfico
 * espera os dados puros.
 * */
void uartRawData(uint16_t * data, uartInterface uartXX)
{
  uint8_t * byteData = (uint8_t*)(data);
  uartSend(*byteData++, uartXX);            // Manda o primeiro byte
  uartSend(*byteData, uartXX);              // Depois o segundo byte
}


