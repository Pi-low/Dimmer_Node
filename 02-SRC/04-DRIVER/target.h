/* 
 * File:   target.h
 * Author: Nello
 *
 * Created on March 21, 2021, 10:16 PM
 */

#ifndef TARGET_H
#define	TARGET_H

#include <xc.h>
#define _XTAL_FREQ 16000000UL
#define LED_PIN LATCbits.LATC4

typedef union
{
    uint8_t EE_ARRAY[16];
    struct
    {
        uint8_t InitFlag;
        uint8_t RX_P0[5];
        uint8_t RX_P1[4];
        uint8_t TX_ADDR[4];
        uint8_t FCY_Chan;
        uint8_t UID;
    }s;
}EE_Map;

extern uint16_t Tick_1ms;
extern EE_Map EEPROM_Row;

void MCU_Init(void);
void NVM_Init(void);
uint16_t GetTick_1ms(void);
uint8_t SPI_Exchange(uint8_t Data);
uint16_t AcquireADCChan(uint8_t Channel);
uint8_t NVM_Read(uint16_t address);
void NVM_Read_Buff(uint16_t address, uint8_t RBuff[], uint8_t length);
void NVM_Erase(uint16_t address);
void NVM_Write_Row(uint16_t address, uint8_t WriteBuff[], uint8_t Length);
void SetPWM1(uint16_t DutyCycle);
void SetPWM2(uint16_t DutyCycle);

#endif	/* TARGET_H */

