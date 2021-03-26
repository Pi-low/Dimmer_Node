#include <xc.h>
#include "../01-GLOBALS/common.h"
#include "target.h"

uint16_t Tick_1ms;

void MCU_Init(void) {
    Tick_1ms = 0u;
    OSCCON = 0x78u;
    
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x44;
    
    TRISC = 0x44u;
    TRISB = 0x3Fu;
    TRISA = 0xFFu;
    
    SSP1STAT = 0x00;
    SSP1CON1 = 0x22;
    
    OPTION_REG = 0x83;
    TMR0 = 5u;
    INTCONbits.T0IE = 1u;
    INTCONbits.PEIE = 1u;
    INTCONbits.GIE = 1u;
}

uint16_t GetTick_1ms(void) {
    return Tick_1ms;
}

void __interrupt() ISR (void) {
    if (TMR0IF == 1u) {
        TMR0 = 5u;
        Tick_1ms++;
        TMR0IF = 0u;
    }
}

uint8_t SPI_Exchange(uint8_t Data) {
    SSP1BUF = Data;
    while (!SSP1STATbits.BF) {
    }
    return SSP1BUF;
}
