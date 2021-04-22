#include <xc.h>
#include <stdint.h>
#include "../01-GLOBALS/common.h"
#include "../02-APPLICATION/Appl.h"
#include "../04-DRIVER/target.h"
#include "../04-DRIVER/NRF24L01/NRF24L01.h"
#include "network.h"

uint8_t DataPipe[2][8] = {{0U}, {0U}};
uint8_t DataTx[8] = {0U};
u_Flags NwtFlags;

static uint8_t CmdPWM1 = 0U;
static uint8_t CmdPWM2 = 0U;
static uint8_t Slope1 = 0U;
static uint8_t Slope2 = 0U;

void NetworkInit(void) {
    uint8_t i = 0;
    uint8_t AddrPipe0[5] = {0};
    uint8_t AddrPipe1[5] = {0};
    uint8_t TxAddr[5] = {0};
    uint8_t EEPROM_Row[NB_WORDS_PER_ROW] = {0};
    
    NVM_Read_Buff(EEPROM_BASE_ADDR, EEPROM_Row, NB_WORDS_PER_ROW);
    
    for (i = 0; i < 5; i++) {
        if (i < 4) {
            AddrPipe1[i] = EEPROM_Row[i + 6];
            TxAddr[i] = EEPROM_Row[i + 10];
        }
        AddrPipe0[i] = EEPROM_Row[i + 1];
    }
    AddrPipe1[4] = EEPROM_Row[15];
    TxAddr[4] = EEPROM_Row[15];
    
    NRF24L01_Init(SPI_Exchange);
    NRF_SetCRCLen(1U);
    NRF_SetAddrWidth(5U);
    NRF_SetRFDataRate(NRF_1MBPS);
    NRF_SetRFPower(NRF_PWR_MAX);
    NRF_SetRFChannel(EEPROM_Row[14]);
    NRF_OpenReadingPipe(PIPE0, AddrPipe0, 8U, 1U, 1U);
    NRF_OpenReadingPipe(PIPE1, AddrPipe1, 8U, 0U, 1U);
    NRF_SetTxAddr(TxAddr);
    NRF_SetART(10U, 4U);
    NRF_StartListening();
    Slope1 = 50;
    CmdPWM1 = 255;
}

void NetworkManager(void) {
    /* Broadcast address */
    if (NwtFlags.P0_Rx != 0U) {
        CmdPWM1 = DataPipe[0][DEVICE_ADDR * 2U];
        CmdPWM2 = DataPipe[0][(DEVICE_ADDR * 2U) + 1U];
        NwtFlags.P1_Rx = 0U;
    }
    else {
    }
    
    /* Node address */
    if (NwtFlags.P1_Rx != 0U) {
        switch (DataPipe[1][0]) {
            /* Set lights */
            case 0x01:
                CmdPWM1 = DataPipe[0][1];
                Slope1 = DataPipe[0][2];
                CmdPWM2 = DataPipe[0][3];
                Slope2 = DataPipe[0][4];
                break;
                
            /* Read lights */
            case 0x02:
                DataTx[0] = 0x02;
                DataTx[1] = CmdPWM1;
                DataTx[2] = Slope1;
                DataTx[3] = CmdPWM2;
                DataTx[4] = Slope2;
                DataTx[5] = 0;
                DataTx[6] = 0;
                DataTx[7] = 0;
                NwtFlags.Tx_Dat = 1;
                break;
                
            /* Read Voltage */    
            case 0x03:
                DataTx[0] = 0x03;
                DataTx[1] = (uint8_t) GetVoltage();
                DataTx[2] = (uint8_t) (GetVoltage() >> 8);
                DataTx[3] = 0;
                DataTx[4] = 0;
                DataTx[5] = 0;
                DataTx[6] = 0;
                DataTx[7] = 0;
                NwtFlags.Tx_Dat = 1;
                break;
        }
        NwtFlags.P0_Rx = 0U;
    } else {
    }
    
    if (NwtFlags.Tx_Dat != 0) {
        NRF_StopListening();
        NRF_WritePayload(DataTx, 8);
        NRF_StartListening();
        NwtFlags.Tx_Dat = 0;
    }
    else {
    }
}

uint8_t Get_PWM1Cmd(void) {
    return CmdPWM1;
}

uint8_t Get_PWM2Cmd(void) {
    return CmdPWM2;
}

uint8_t Get_Slope1(void) {
    return Slope1;
}

uint8_t Get_Slope2(void) {
    return Slope2;
}