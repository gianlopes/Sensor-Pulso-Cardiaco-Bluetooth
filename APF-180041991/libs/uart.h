/*
 * uart.h
 * Pinos P4.4 e P4.5
 */

#ifndef UART_H
#define UART_H

#include <msp430.h>
#include <stdint.h>

typedef enum {uartPC, uartBT} uartInterface;

/*
 * UART usada na comunica��o com o PC
 * Configura��o da USCI-A1
 * Conven��o usada:
 * baudrate = 19200
 * LSB-first
 * sem paridade
 * stop bits = 1
 */
/*
 * UART usada no BlueTooth
 * Configura��o da USCI-A0
 * Conven��o usada:
 * baudrate = 9600
 * LSB-first
 * sem paridade
 * stop bits = 1
 */
void uartInit();
void uartSend(uint8_t byte, uartInterface uartXX);
void uartPrint(char * str, uartInterface uartXX);
void uartPrintChar(char c, uartInterface uartXX);
void uartDec8(uint8_t num, uartInterface uartXX);
void uartDec16(uint16_t num, uartInterface uartXX);
void uartRawData(uint16_t * data, uartInterface uartXX);

#endif /* UART_H */
