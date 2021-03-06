#include <xc.h>
#include "../01-GLOBALS/common.h"
#include "../02-APPLICATION/Appl.h"
#include "../03-NETWORK/network.h"
#include "NRF24L01/NRF24L01.h"
#include "target.h"

uint16_t Tick_1ms;
EE_Map EEPROM_Row;
static void NWM_Unlock (void);

void MCU_Init(void)
{
    Tick_1ms = 0;
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
    SetPWM1(0);
    SetPWM2(0);
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

void NVM_Init(void)
{
    NVM_Read_Buff(EEPROM_BASE_ADDR, EEPROM_Row.EE_ARRAY, 16);
    
    if (EEPROM_Row.s.InitFlag != 0xA5)
    {
        /* 1F80 */
        EEPROM_Row.s.InitFlag = 0xA5; /* Present config flag */
        /* PIPE0 addr
         * @1F81: 5 bytes */
        EEPROM_Row.EE_ARRAY[1] = 'B';
        EEPROM_Row.EE_ARRAY[2] = 'R';
        EEPROM_Row.EE_ARRAY[3] = 'O';
        EEPROM_Row.EE_ARRAY[4] = 'A';
        EEPROM_Row.EE_ARRAY[5] = 'D';
        
        /* PIPE 1
         * @1F86: 4bytes */
        EEPROM_Row.EE_ARRAY[6] = 'N';
        EEPROM_Row.EE_ARRAY[7] = 'O';
        EEPROM_Row.EE_ARRAY[8] = 'D';
        EEPROM_Row.EE_ARRAY[9] = 'E';
        
        /* TX ADDR 
         * @1F8B: 4bytes */
        EEPROM_Row.EE_ARRAY[10] = 'L';
        EEPROM_Row.EE_ARRAY[11] = 'E';
        EEPROM_Row.EE_ARRAY[12] = 'D';
        EEPROM_Row.EE_ARRAY[13] = 'X';
        
        /* General config
         * @1F8E */
        EEPROM_Row.s.FCY_Chan = 100;
        EEPROM_Row.s.UID = DEVICE_ADDR;
        NVM_Write_Row(EEPROM_BASE_ADDR, EEPROM_Row.EE_ARRAY, 16);
    }
}

uint16_t GetTick_1ms(void)
{
    return Tick_1ms;
}

void SetPWM1(uint16_t DutyCycle)
{
    PWM1DCL = (uint8_t) DutyCycle << 6;
    PWM1DCH = (uint8_t) (DutyCycle >> 2);
}

void SetPWM2(uint16_t DutyCycle)
{
    PWM2DCL = (uint8_t) DutyCycle << 6;
    PWM2DCH = (uint8_t) (DutyCycle >> 2);
}

uint8_t SPI_Exchange(uint8_t Data)
{
    SSP1BUF = Data;
    while (!SSP1STATbits.BF);
    return SSP1BUF;
}

uint16_t AcquireADCChan(uint8_t Channel)
{
    uint16_t ADCres10 = 0;
    ADCON0bits.CHS = Channel;
    ADCON0bits.ADON = 1;
    __delay_us(100);
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO != 0);
    ADCres10 = (uint16_t)(ADRESH << 8) + ADRESL;
    return ADCres10;
}

uint8_t NVM_Read(uint16_t address)
{
    PMCON1bits.CFGS = 0;
    PMADRH = (uint8_t) (address >> 8);
    PMADRL = (uint8_t) address;
    PMCON1bits.RD = 1;
    NOP();
    NOP();
    return PMDATL;
}

void NVM_Read_Buff(uint16_t address, uint8_t RBuff[], uint8_t length)
{
    uint8_t i = 0;
    for (i = 0;  i < length; i++)
    {
        RBuff[i] = NVM_Read(address + i);
    }
}

void NWM_Unlock(void)
{
    PMCON2 = 0x55;
    PMCON2 = 0xAA;
    PMCON1bits.WR = 1;
    NOP();
    NOP();
}

void NVM_Erase(uint16_t address)
{
    di(); /* Disable interrupt */
    PMCON1bits.CFGS = 0;
    PMADRH = (uint8_t) (address >> 8);
    PMADRL = (uint8_t) address;
    PMCON1bits.FREE = 1;
    PMCON1bits.WREN = 1;
    NWM_Unlock(); /* Erase takes approx 2ms */
    PMCON1bits.WREN = 0;
    ei(); /* Enable interrupt */
}

void NVM_Write_Row(uint16_t address, uint8_t WriteBuff[], uint8_t Length)
{
    uint8_t i = 0;
    if (Length < NB_WORDS_PER_ROW && !(address % NB_WORDS_PER_ROW))
    {
        di(); /* Disable interrupt */
        PMADRH = (uint8_t) (address >> 8);
        PMADRL = (uint8_t) address;
        PMCON1bits.CFGS = 0;
        PMCON1bits.WREN = 1;
        PMCON1bits.LWLO = 1;
        
        /* Load latched */
        while (i < Length)
        {
            PMDATL = WriteBuff[i];
            PMDATH = 0;
            NWM_Unlock();
            i++;
            PMADRL += (i < Length);
        }
        PMCON1bits.LWLO = 0;
        NWM_Unlock(); /* Write all latched into EEPROM */
        PMCON1bits.WREN = 0;
        ei(); /* Enable interrupt */
    }
}

void __interrupt() ISR (void)
{
    uint8_t PipeNo = 0;
    if (TMR0IF == 1)
    {
        TMR0 = 5;
        Tick_1ms++;
        TMR0IF = 0;
    }
    else if (IOCIF == 1)
    {
        /* New data available */
        ClickLed(50);
        PipeNo = NRF_Available();
        NRF_ReadPayload(DataPipe[PipeNo], 8);
        NwtFlags.Byte = 1 << PipeNo;
        IOCAF = 0;
        IOCIF = 0;
    }
}
