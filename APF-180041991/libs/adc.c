#include "adc.h"
#include "uart.h"
#include <stdbool.h>

/*
 * Esta biblioteca faz as medidas do sensor de pulso card�aco utilizando o AD e trata
 * os valores recebidos para detectar pulsos.
 *
 * O sensor � bem impreciso, por isso o c�digo para tratar os valores ficou bem
 * complexo.
 *
 * O c�digo que trata os valores est� na interrup��o do AD, ele foi baseado no c�digo
 * para arduino, fornecido pelo fabricante, para melhorar a precis�o baixa do sensor,
 * dispon�vel em https://pulsesensor.com/pages/pulse-sensor-amped-arduino-v1dot1
 *
 */

void adcInit()
{
    /* AD Config */
    pinMode(6, 0, input, PSEL_1);           // P6.0 � o pulso do sensor

    ADC12CTL0 &= ~ADC12ENC;                 // Desativa o trigger
    REFCTL0 = 0;                            // Desliga o m�dulo de refer�ncia
    ADC12CTL0 = ADC12SHT0_8      |          // 64 batidas de clock
//                ADC12MSC         |          // Multiplas convers�es por trigger
                ADC12ON;                    // Liga o quantizador
    ADC12CTL1 = ADC12CSTARTADD_0 |          // Resultado --> MEM0
                ADC12SHS_0       |          // Trigger: bit sc
                ADC12SHP         |          // Usa o timer interno
                ADC12SSEL_0      |          // Clock de 5Mhz
                ADC12CONSEQ_0;              // Single Channel Conv
    ADC12CTL2 = ADC12TCOFF       |          // Desliga o sensor de temp
                ADC12RES_2;                 // 12 bits de resolu��o

    // Configura uma medida no canal 0
    ADC12MCTL0 = ADC12INCH_0 | ADC12SREF_0 | ADC12EOS;

    ADC12IE = BIT0;                         // Interrup��o no final da sequ�ncia


    /* Timer config */

    /* Timer para dar trigger no ADC */
    TA0CTL = TASSEL__ACLK | MC__UP | TACLR; // ACLK = 32768 Hz
    TA0CCR0 = 66 - 1;                       // ~= 500 Hz de amostragem
    TA0CCTL0 = CCIE;

    /* Timer para contar tempo */
    TA1CCR0 = 1049 - 1;
    TA1CTL = TASSEL__SMCLK |                // Configura o passo para 1us
             MC__UP        |                // Modo up, para o usu�rio configurar o teto
             TACLR;
    TA1CCTL0 = CCIE;

}


/* Vari�veis */
uint16_t sample;                            // Armazena uma amostra do AD

uint16_t intervals[8];                      // Armazena os �ltimos 8 intervalos entre medidas
uint8_t  intervals_i = 0;                   // Usado para iterar no vetor intervals

/* Vari�veis que guardam tempo */
const uint16_t sampleInterval = 2;          // 500 hz de amostragem = 2ms

uint32_t globalTime = 0;
uint16_t beatInterval = 750;                // Intervalo entre pulsos. Padr�o 750ms=80bpm
uint32_t lastBeat = 0;                      // Armazena o tempo em que come�ou a �ltima batida
uint16_t timeSinceLastBeat = 0;             // Tempo desde a �ltima batida detectada (intervalo)

/* Controle do buzzer */
bool buzzer = false;                        // Indicar que o buzzer est� ativo
uint32_t timerBuzzer = 0;                   // At� qual tempo o buzzer deve ficar ativo

/* Flags */
bool pulse = false;                         // para indicar que est� em um pulso
bool firstBeat = true;                      // O primeiro pulso ser� ignorado nas medidas
bool secondBeat = false;

uint8_t loopAux;                            // Usado para iterar nos loops

/* Vari�veis relacionadas a forma de onda.
 * S�o utilizados para atualizar dinamicamente o limiar de detec��o de pulso
 */
uint16_t thresh = 2055;     //+- meio do ad (apartir de quanto uma batida � considerada)
uint16_t pico = 2055;
uint16_t vale = 2055;
uint16_t amplitude = 400;                   // 10% da resolu��o do AD


/* Interrup��o do ADC */
#pragma vector = ADC12_VECTOR
__interrupt void ADC_ISR()
{
    switch (ADC12IV)
    {
        case ADC12IV_ADC12IFG0:
        {
            sample = ADC12MEM0;
            uartRawData(&sample, uartPC);

            timeSinceLastBeat = globalTime - lastBeat;

            /* Verifica se o timer do buzzer j� acabou */
            if(timerBuzzer < globalTime)
            {
                buzzer = false;
            }


            /* Evitar noise esperando 3/5 do �ltimo intervalo entre pulsos */
            if((sample < thresh) && (timeSinceLastBeat > (beatInterval / 5)*3))
            {
                if(sample < vale)
                    vale = sample;          // Guarda o ponto mais baixo do sinal
            }

            if(sample > thresh && sample > pico)
            {
                pico = sample;              // Guarda o ponto mais alto do sinal
            }

            if(timeSinceLastBeat > 375)     //Evita noise de alta frequ�ncia (375ms=160bpm)
            {
                /* Pulso detectado */
                if(sample > thresh && pulse == 0 && (timeSinceLastBeat > (beatInterval/5)*3))
                {
                    P1OUT |= BIT0;          // Ativa o led
                    buzzer = true;          // Ativa o buzzer
                    timerBuzzer = globalTime + 200;

                    pulse = true;
                    beatInterval = globalTime - lastBeat;
                    intervals[intervals_i] = beatInterval;
                    intervals_i = (intervals_i + 1) & 0x7;  // Incrementa o iterador circular

                    lastBeat = globalTime;
                }
                /* O segundo pulso � usado para resetar os valores antigos de intevalo */
                if(secondBeat)
                {
                    secondBeat = false;
                    for(loopAux = 0; loopAux < 8; loopAux++)
                    {
                        intervals[loopAux] = beatInterval;
                    }
                }
                /* O primeiro pulso � ignorado */
                if(firstBeat)
                {
                    firstBeat = false;
                    secondBeat = true;
                    return;
                }

            }
            /* Atualiza o limiar do pulso dinamicamente */
            if(sample < thresh && pulse == true)
            {
                pulse = false;
                P1OUT &= ~BIT0;
                amplitude = pico - vale;    // amplitude das batidas
                thresh = amplitude / 2 + vale;// thresh � metade da amplitude
                pico = thresh;
                vale = thresh;
            }
            /* Se ficar muito tempo (2.5s) sem ter um pulso, reseta os valores */
            if(timeSinceLastBeat > 2500)
            {
                thresh = 2055; //+- meio do ad (apartir de quanto uma batida � considerada)
                pulse = false;
                lastBeat = 0;
                globalTime = 0;
                beatInterval = 750;         // Intervalo entre duas medidas 750 ms = 80bpm
                pico = 2055;
                vale = 2055;
                amplitude = 400;            // 1/10 do range de entradas
                firstBeat = true;
                secondBeat = false;
            }
            break;
        }
        default:
            break;
    }
}

/* Interrup��o do timer para habilitar o AD */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_CCR0_ISR()
{
    ADC12CTL0 |= ADC12ENC;                  // Habilita o conversor
    ADC12CTL0 &= ~ADC12SC;                  // Gera um flanco de subida em SC
    ADC12CTL0 |= ADC12SC;
}

/* Timer contador de tempo em ms
 * Aproveita para fazer o buzzer soar a na frequencia de 500Hz*/
#pragma vector = TIMER1_A0_VECTOR
__interrupt void TA1_CCR0_ISR()
{
    globalTime++;                           // Incrementa 1 ms

    if(buzzer)
    {
        P8OUT ^= BIT1;
    }
}
