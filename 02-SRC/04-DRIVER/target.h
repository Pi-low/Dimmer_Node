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

extern uint16_t Tick_1ms;

void MCU_Init(void);
uint16_t GetTick_1ms(void);
uint8_t SPI_Exchange(uint8_t Data);
uint16_t AcquireADCChan(uint8_t Channel);
void SetPWM1(uint16_t DutyCycle);
void SetPWM2(uint16_t DutyCycle);

#endif	/* TARGET_H */

