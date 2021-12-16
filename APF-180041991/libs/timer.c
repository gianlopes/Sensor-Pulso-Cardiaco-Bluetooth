#include "timer.h"

void wait(uint16_t time, timeUnit_t unit)
{
    int i;
    switch (unit) {
        case us:
            if(time == 1)
                TA2CCR0 = time;
            else
                TA2CCR0 = time - 1;

            TA2EX0 = TAIDEX_0;          // Não vai dividir o clock
            TA2CTL = TASSEL__SMCLK |    // Configura o passo para 1us
                     MC__UP        |    // Modo up, para o usuário configurar o teto
                     TACLR;

            while(!(TA2CCTL0 & CCIFG));
            TA2CCTL0 &= ~CCIFG;

            TA2CTL = MC__STOP;  // Trava o timer

            break;

        case ms:
            TA2EX0 = TAIDEX_3;         // divide o clock por 4

            if(time == 1)
                TA2CCR0 = time;
            else
                TA2CCR0 = time - 1;

            TA2CTL = TASSEL__ACLK |    // Clock de 32KHz
                     MC__UP       |    // Modo up, para o usuário configurar o teto
                     ID__8        |    // divide o clock por 8
                     TACLR;

            // (32 KHz / 4) / 8 = 1KHz (passo de aproximadamente 1 ms (0.977 ms))

            while(!(TA2CCTL0 & CCIFG));
            TA2CCTL0 &= ~CCIFG;

            TA2CTL = MC__STOP;  // Trava o timer
            TA2EX0 = TAIDEX_0;  // Reseta o divisor de clock

            break;

        case sec:
            TA2EX0 = TAIDEX_0;       // não divide o clock

            TA2CTL = TASSEL__ACLK |  // Clock de 32KHz (32768)
                     MC__UP       |  // Modo up, para o usuário configurar o teto
                     TACLR;

            TA2CCR0 = 0x8000 - 1; // Teto de 1 seg

            for(i = 0; i < time; i++)
            {
                // 1 segundo de atraso
                while(!(TA2CCTL0 & CCIFG));
                TA2CCTL0 &= ~CCIFG;
            }

            TA2CTL = MC__STOP;  // Trava o timer

            break;
        default:
            break;
    }
    return;
}
