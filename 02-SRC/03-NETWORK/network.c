#include <xc.h>
#include <stdint.h>
#include "../01-GLOBALS/common.h"
#include "../02-APPLICATION/Appl.h"
#include "../04-DRIVER/target.h"
#include "../04-DRIVER/NRF24L01/NRF24L01.h"
#include "network.h"

uint8_t DataPipe[2][8] = {{0}, {0}};
uint8_t DataTx[8] = {0};
u_Flags NwtFlags;
s_Addresses NwtAddr;

static uint8_t CmdPWM1 = 0;
static uint8_t CmdPWM2 = 0;
static uint8_t Slope1 = 0;
static uint8_t Slope2 = 0;
static uint32_t TxTimeout = 0;

void NetworkInit(void)
{
    uint8_t u8i = 0;
    NwtFlags.Byte = 0;
    
    for (u8i = 0; u8i < 5; u8i++)
    {
        NwtAddr.P0Addr[u8i] = EEPROM_Row.s.RX_P0[u8i];
        if (u8i < 4)
        {
            NwtAddr.P1Addr[u8i] = EEPROM_Row.s.RX_P1[u8i];
            NwtAddr.TxAddr[u8i] = EEPROM_Row.s.TX_ADDR[u8i];
        }
    }
    NwtAddr.P1Addr[4] = EEPROM_Row.s.UID;
    NwtAddr.TxAddr[4] = EEPROM_Row.s.UID;
    
    NRF24L01_Init(SPI_Exchange);
    NRF_SetCRCLen(1);
    NRF_SetAddrWidth(5);
    NRF_SetRFDataRate(NRF_1MBPS);
    NRF_SetRFPower(NRF_PWR_MAX);
    NRF_SetRFChannel(EEPROM_Row.s.FCY_Chan);
    NRF_OpenReadingPipe(PIPE0, NwtAddr.P0Addr, 8, 1, 1);
    NRF_OpenReadingPipe(PIPE1, NwtAddr.P1Addr, 8, 0, 1);
    NRF_SetTxAddr(NwtAddr.TxAddr);
    NRF_SetART(10, 4);
    NRF_StartListening();
    Slope1 = 50;
    CmdPWM1 = 255;
}

void NetworkManager(void)
{
    uint8_t Payload[8] = {0};
    /* Broadcast address */
    if (NwtFlags.s.P0_Rx != 0)
    {
        CmdPWM1 = DataPipe[0][EEPROM_Row.s.UID * 2];
        CmdPWM2 = DataPipe[0][(EEPROM_Row.s.UID * 2) + 1];
        NwtFlags.s.P0_Rx = 0;
    }
    
    /* Node address */
    if (NwtFlags.s.P1_Rx != 0)
    {
        switch (DataPipe[1][0])
        {
            /* Set lights */
            case 0x01:
                CmdPWM1 = DataPipe[0][1];
                Slope1 = DataPipe[0][2];
                CmdPWM2 = DataPipe[0][3];
                Slope2 = DataPipe[0][4];
                break;
                
            /* Increment */    
            case 0x02:
                CmdPWM1 += (DataPipe[0][1] + CmdPWM1 < CmdPWM1) ? 255 : DataPipe[0][1];
                CmdPWM2 += (DataPipe[0][2] + CmdPWM2 < CmdPWM2) ? 255 : DataPipe[0][2];
                break;
                
            /* Decrement */    
            case 0x03:
                CmdPWM1 -= (DataPipe[0][1] - CmdPWM1 > CmdPWM1) ? 0 : DataPipe[0][1];
                CmdPWM2 -= (DataPipe[0][2] - CmdPWM2 > CmdPWM2) ? 0 : DataPipe[0][2];
                break;
                
            /* Read lights */
            case 0x04:
                Payload[0] = 0x04;
                Payload[1] = CmdPWM1;
                Payload[2] = Slope1;
                Payload[3] = CmdPWM2;
                Payload[4] = Slope2;
                QueueMessage(Payload);
                break;
                
            /* Read Voltage */    
            case 0x05:
                Payload[0] = 0x05;
                Payload[1] = (uint8_t) GetVoltage();
                Payload[2] = (uint8_t) (GetVoltage() >> 8);
                QueueMessage(Payload);
                break;
                
            /*  Set Node ID*/
            case 0x06:
                Payload[0] = 0x06;
                Payload[1] = DataPipe[1][1];
                EEPROM_Row.s.UID = DataPipe[1][1];
                NwtFlags.s.EE_Wr = 1;
                QueueMessage(Payload);
                break;
                
            /* Set Node frequency */    
            case 0x07:
                Payload[0] = 0x07;
                Payload[1] = DataPipe[1][1];
                EEPROM_Row.s.FCY_Chan = DataPipe[1][1];
                NwtFlags.s.EE_Wr = 1;
                QueueMessage(Payload);
                break;
                
            default:
                break;
        }
        NwtFlags.s.P1_Rx = 0;
    }
    
    if (NwtFlags.s.EE_Wr != 0)
    {
        NVM_Erase(EEPROM_BASE_ADDR);
        NVM_Write_Row(EEPROM_BASE_ADDR, EEPROM_Row.EE_ARRAY, 16);
        NwtFlags.s.EE_Wr = 0;
    }
}

void DataTxManager(void)
{
    if ((NwtFlags.s.Tx_Dat != 0) && (Tick_1ms > TxTimeout))
    {
        NRF_StopListening();
        NRF_WritePayload(DataTx, 8);
        NRF_StartListening();
        NwtFlags.s.Tx_Dat = 0;
    }
}

uint8_t Get_PWM1Cmd(void)
{
    return CmdPWM1;
}

uint8_t Get_PWM2Cmd(void)
{
    return CmdPWM2;
}

uint8_t Get_Slope1(void)
{
    return Slope1;
}

uint8_t Get_Slope2(void)
{
    return Slope2;
}

void QueueMessage(uint8_t Buffer[])
{
    uint8_t u8i;
    for (u8i = 0; u8i < 8; u8i++)
    {
        DataTx[u8i] = Buffer[u8i];
    }
    NwtFlags.s.Tx_Dat = 1;
    TxTimeout = Tick_1ms + 10;
}