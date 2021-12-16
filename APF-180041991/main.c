/**
 * Projeto Final
 *
 * Gianlucas Dos Santos Lopes - 180041991
 * MSP430F5529
 *
 * Utiliza o sensor cardíaco para medir BPM em enviar via bluetooth.
 * utilizando o módulo bluetooth HC-05.
 * O buzzer faz um pequeno som quando um pulso está em andamento.
 * O led vermelho acende quando um pulso está em andamento.
 * O lcd é usado também para mostrar os BPM.
 *
 * P4.4 = UCA1TX - UART com o pc para plotar gráfico
 * P4.5 = UCA1RX - UART com o pc para plotar gráfico
 * P3.3 = UCA0TX - UART para bluetooth
 * P3.4 = UCA0RX - UART para bluetooth
 * P8.1 = Enable do Buzzer
 * P4.1 = SDA - I2C LCD
 * P4.2 = SCL - I2C LCD
 * P6.0 = ADC12 canal 0 sinal do sensor cardíaco
 */

#include <msp430.h>
#include <stdint.h>
#include "libs/gpio.h"
#include "libs/timer.h"
#include "libs/adc.h"
#include "libs/uart.h"
#include "libs/lcd.h"

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	            // stop watchdog timer
	
	volatile uint32_t time;
	volatile uint16_t BPM;
	volatile uint8_t i;

	pinMode(1, 0, output, PSEL_0);          // Led vermelho
	pinMode(8, 1, output, PSEL_0);          // Pino do buzzer

	adcInit();
	uartInit();
	lcdInit();

	__enable_interrupt();

	while (1)
    {
	    wait(1, sec);

	    /* Calculando média móvel */
	    time = 0;
	    for(i = 0; i < 8; i++)
	    {
	        time += intervals[i];
	    }
	    time = time / 8;                    // divide por 8 pra fazer a média
	    BPM = 60000 / time;                 // 60s/time = 60000ms/time = BPM

	    /* Mandando BPM pelo Bluetooth */
	    uartPrint("BPM:", uartBT);
	    uartDec16(BPM, uartBT);
	    uartPrint("\n", uartBT);

	    /* Mostrando BPM no lcd */
	    lcdClear();
	    lcdPrint("   BPM:");
	    lcdDec16(BPM);
    }
}



