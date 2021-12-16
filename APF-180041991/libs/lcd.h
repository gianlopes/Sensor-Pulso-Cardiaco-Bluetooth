#ifndef LCD_H
#define LCD_H

#include <msp430.h>
#include <stdint.h>
#include "i2c.h"
#include "timer.h"
#define lcdAddr 0x3F

void lcdGetByte(uint8_t * byte);

/* Inicia o lcd */
void lcdInit();

/* Imprime um char qualquer */
void lcdPrintChar(char c);

/* lcdPrint espera uma string terminada em \0 */
void lcdPrint(char * str);

/* Fun��es para mudar para o in�cio de uma linha */
void lcdLinha0();
void lcdLinha1();

/* lcdCursor muda a posi��o do cursor
   lin : 0 a 1
   col : 0 a 15 */
void lcdCursor(uint8_t lin, uint8_t col);

/* Fun��es para alterar a dire��o de movimento do cursor */
void lcdDirectionRight();
void lcdDirectionLeft();

/* Alternam a visibilidade do cursor */
void lcdDisableCursor();
void lcdEnableCursor();

/* Limpa a ddram e reseta o cursor para a primeira posi��o */
void lcdClear();

/* Fun��o para criar um novo char na mem�ria do lcd
 * addr : 0 a 7
 * desenho: vetor com 8 chars (cada char � um linha com 5 bits) */
void lcdNewChar(uint8_t addr, char * desenho);

/* Fun��es para imprimir n�meros em diferentes formatos no lcd */
void lcdDec8(uint8_t num);
void lcdDec16(uint16_t num);
void lcdHex8(uint8_t num);
void lcdHex16(uint16_t num);
void lcdFloat(float x);
void lcdFloat1000(uint16_t x);


#endif /* LCD_H */
