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
    
    /* SPI */
    SSP1STAT = 0x40u; /* CKE = 1 */
    SSP1CON1 = 0x22u; /* CKP = 0, SPI MASTER 250Kbps */
    
    /* TIMER0 */
    OPTION_REG = 0x83u;
    TMR0 = 5u;
    
    /* TIMER2*/
    PR2 = 255u;
    T2CON = 0x04u;
    
    /* PWM */
    PWM1CON = 0xC0;
    PWM2CON = 0xC0;
    
    INTCONbits.T0IE = 1u;
    INTCONbits.INTE = 0u;
    INTCONbits.PEIE = 1u;
    INTCONbits.GIE = 1u;
}

uint16_t GetTick_1ms(void) {
    return Tick_1ms;
}

void SetPWM1(uint16_t DutyCycle) {
    PWM1DCL = (uint8_t) DutyCycle << 6u;
    PWM1DCH = (uint8_t) (DutyCycle >> 2u);
}

void SetPWM2(uint16_t DutyCycle) {
    PWM2DCL = (uint8_t) DutyCycle << 6u;
    PWM2DCH = (uint8_t) (DutyCycle >> 2u);
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
    while (!SSP1STATbits.BF);
    return SSP1BUF;
}
