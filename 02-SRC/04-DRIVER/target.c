#include <xc.h>
#include "../01-GLOBALS/common.h"
#include "target.h"
#include "NRF24L01/NRF24L01.h"
#include "../02-APPLICATION/Appl.h"

uint16_t Tick_1ms;
uint8_t NRF_Payload[8] = {0};

void MCU_Init(void) {
    Tick_1ms = 0u;
    OSCCON = 0x78;
    
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x44;
    
    TRISC = 0x44;
    TRISB = 0x3F;
    TRISA = 0xFF;
    
    /* SPI */
    SSP1STAT = 0x40; /* CKE = 1 */
    SSP1CON1 = 0x21; /* CKP = 0, SPI MASTER 1Mbps */
    
    /* TIMER0
     * Set as 1ms tick */
    OPTION_REG = 0x83;
    TMR0 = 5;
    INTCONbits.T0IE = 1;
    
    /* TIMER2 
     * Set for PWM period */
    PR2 = 255;
    T2CON = 0x04;
    
    /* PWM */
    PWM1CON = 0xC0;
    PWM2CON = 0xC0;
    
    /* Interrupts on change */
    IOCAN = 0x04; /* RA2 interrupt on falling edge  */
    INTCONbits.IOCIE = 1;
    INTCONbits.IOCIF = 0;
    
    /* Global interrupt */
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

uint16_t GetTick_1ms(void) {
    return Tick_1ms;
}

void SetPWM1(uint16_t DutyCycle) {
    PWM1DCL = (uint8_t) DutyCycle << 6u;
    PWM1DCH = (uint8_t) (DutyCycle >> 2);
}

void SetPWM2(uint16_t DutyCycle) {
    PWM2DCL = (uint8_t) DutyCycle << 6u;
    PWM2DCH = (uint8_t) (DutyCycle >> 2);
}

uint8_t SPI_Exchange(uint8_t Data) {
    SSP1BUF = Data;
    while (!SSP1STATbits.BF);
    return SSP1BUF;
}

void __interrupt() ISR (void) {
    if (TMR0IF == 1) {
        TMR0 = 5;
        Tick_1ms++;
        TMR0IF = 0;
    }
    else if (IOCIF == 1) {
        ClickLed(100U);
        if (NRF_Available(PIPE0)) {
            NRF_ReadPayload(NRF_Payload, 8);
        }
        NRF_Write_Register(REG_NRF_STATUS, 0x70, 1);
        IOCAF = 0;
        IOCIF = 0;
    }
}
