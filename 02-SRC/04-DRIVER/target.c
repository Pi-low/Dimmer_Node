#include <xc.h>
#include "../01-GLOBALS/common.h"
#include "../02-APPLICATION/Appl.h"
#include "../03-NETWORK/network.h"
#include "NRF24L01/NRF24L01.h"
#include "target.h"

uint16_t Tick_1ms;

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
    
    /* ADC */
    ADCON1 = 0xA0;
    ADCON0 |= 1;
    
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

uint16_t AcquireADCChan(uint8_t Channel) {
    uint16_t ADCres10 = 0;
    ADCON0 = (Channel << 2) & 0xFD;
    __delay_us(100);
    ADCON0 |= 2;
    while ((ADCON0 & 2) == 0);
    ADCres10 = (uint16_t)(ADRESH << 8) + ADRESL;
    return ADCres10;
}

void __interrupt() ISR (void) {
    uint8_t PipeNo = 0;
    if (TMR0IF == 1) {
        TMR0 = 5;
        Tick_1ms++;
        TMR0IF = 0;
    }
    else if (IOCIF == 1) {
        ClickLed(50);
        PipeNo = NRF_Available();
        NRF_ReadPayload(DataPipe[PipeNo], 8);
        NwtFlags.Byte = 1U << PipeNo;
        IOCAF = 0;
        IOCIF = 0;
    }
    else {
        
    }
}
