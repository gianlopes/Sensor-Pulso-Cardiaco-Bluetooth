#include "lcd.h"

#define BT BIT3
#define EN BIT2
#define RW BIT1

void lcdWriteNibble(uint8_t nibble, uint8_t rs)
{
    i2cSendByte(lcdAddr, nibble << 4 | BT | 0  | rs);
    i2cSendByte(lcdAddr, nibble << 4 | BT | EN | rs);
    i2cSendByte(lcdAddr, nibble << 4 | BT | 0  | rs);
}
void lcdWriteByte(uint8_t byte, uint8_t rs)
{
    lcdWriteNibble(byte >> 4,   rs);        // mais significativo primeiro
    lcdWriteNibble(byte & 0x0F, rs);        // menos significativo depois
}

void lcdGetNibble(uint8_t * nibble)
{
    i2cSendByte(lcdAddr, 0xF << 4 | BT | 0  | RW | 0); // Precisa do 0xF << 4
    i2cSendByte(lcdAddr, 0xF << 4 | BT | EN | RW | 0);
    i2cGetByte(lcdAddr, nibble);
    i2cSendByte(lcdAddr, 0xF << 4 | BT | 0  | RW | 0);
}

void lcdGetByte(uint8_t * byte)
{
    uint8_t temp = 0;
    lcdGetNibble(&temp);
    *byte = temp & 0xF0;
    lcdGetNibble(&temp);
    *byte |= temp >> 4;
}

void lcdInit()
{
    i2cInit();
    lcdWriteNibble(0x3, 0);                 // Garante o modo 8 bits
    wait(4, ms);
    lcdWriteNibble(0x3, 0);
    wait(100, us);
    lcdWriteNibble(0x3, 0);

    // Como está em 8bits, esse WriteNibble recebe o 0x2 na msh do byte.
    lcdWriteNibble(0x2, 0);                 // Modo 4 bits

    lcdWriteByte(0x08, 0);                  // Desativar LCD
    lcdWriteByte(0x28, 0);                  // Duas linhas e Fonte 5x8
    lcdWriteByte(0x01, 0);                  // Clear Display
    lcdWriteByte(0x06, 0);                  // Cursor move para a direita
    lcdWriteByte(0x0F, 0);                  // Liga o cursor piscando e liga o display
    lcdWriteByte(0x02, 0);                  // Return home

}

void lcdPrintChar(char c)
{
    lcdWriteByte(c, 1);
}

/* Expects \0 terminated string */
void lcdPrint(char * str)
{
    while(*str)
    {
        lcdWriteByte(*str++, 1);
    }
}

void lcdLinha0()
{
    lcdWriteByte(0x80, 0);
}

void lcdLinha1()
{
    lcdWriteByte(0xC0, 0);
}

/* Coluna de 0 a 15
 * Linha 0 a 1 */
void lcdCursor(uint8_t lin, uint8_t col)
{
    uint8_t addr = 0;
    if(lin == 1)
        addr = 0x40;
    addr += col;

    lcdWriteByte(BIT7 | addr, 0);
}

void lcdDirectionRight()
{
    lcdWriteByte(0x06, 0);                  // Cursor move para a direita
}

void lcdDirectionLeft()
{
    lcdWriteByte(0x04, 0);                  // Cursor move para a esquerda
}

void lcdDisableCursor()
{
    lcdWriteByte(0x0C, 0);                  // Desabilita o cursor
}

void lcdEnableCursor()
{
    lcdWriteByte(0x0F, 0);                  // Liga o cursor piscando e liga o display
}

void lcdClear()
{
    lcdWriteByte(0x01, 0);                  // Clear Display and reset ddram addr
}

void lcdNewChar(uint8_t addr, char * desenho)
{
    lcdWriteByte(BIT6 | addr << 3, 0);      // Go to cgram
    uint8_t i = 8;
    while(i--)
        lcdWriteByte(*desenho++, 1);
    lcdWriteByte(BIT7 | 0, 0);              // Return to ddram
}

void lcdDec8(uint8_t num)
{
    uint8_t c, d, u, r;

    c = num / 100;
    r = num % 100;

    d = r / 10;
    r = r % 10;

    u = r;

    lcdPrintChar(c + 0x30);
    lcdPrintChar(d + 0x30);
    lcdPrintChar(u + 0x30);
}

void lcdDec16(uint16_t num)
{
    uint16_t m, c, d, u, r;

    m = num / 1000;
    r = num % 1000;

    c = r / 100;
    r = r % 100;

    d = r / 10;
    r = r % 10;

    u = r;

    lcdPrintChar(m + 0x30);
    lcdPrintChar(c + 0x30);
    lcdPrintChar(d + 0x30);
    lcdPrintChar(u + 0x30);
}

void lcdHex8(uint8_t num)
{
    uint8_t n1, n2;

    n1 = ((num & 0xF0) >> 4);
    n2 = (num & 0x0F);

    if(n1 < 10)
        n1 += 0x30;
    else
        n1 += 0x41 - 10;
    if(n2 < 10)
        n2 += 0x30;
    else
        n2 += 0x41 - 10;
    lcdPrintChar(n1);
    lcdPrintChar(n2);
}

void lcdHex16(uint16_t num)
{
    lcdHex8((num & 0xFF00) >> 8);
    lcdHex8(num & 0x00FF);
}

void lcdFloat(float num)
{
    uint8_t c, i;
    c = (uint8_t) num;
    lcdPrintChar(c + 0x30);
    lcdPrintChar('.');

    i = 3;
    while(i--)
    {
        num -= c;
        num *= 10;
        c = (uint8_t) num;
        lcdPrintChar(c + 0x30);
    }
}

/* Imprime floats multiplicados por mil no formato a,aaa */
void lcdFloat1000(uint16_t num)
{
    uint8_t m, c, d, u;

    m = num / 1000;
    num = num % 1000;

    c = num / 100;
    num = num % 100;

    d = num / 10;
    num = num % 10;

    u = num;

    lcdPrintChar(m + 0x30);
    lcdPrintChar(',');
    lcdPrintChar(c + 0x30);
    lcdPrintChar(d + 0x30);
    lcdPrintChar(u + 0x30);
}
