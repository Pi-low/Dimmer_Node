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
    uint8_t AddrPipe1[5] = {'N', 'o', 'd', 'e', '0'};
    uint8_t ModuleAddr[5] = {0xBAU, 0x5EU, 0xBAU, 0x11U, DEVICE_ADDR};
    NRF24L01_Init(SPI_Exchange);
    NRF_SetCRCLen(1U);
    NRF_SetAddrWidth(5U);
    NRF_SetRFDataRate(NRF_1MBPS);
    NRF_SetRFPower(NRF_PWR_MAX);
    NRF_SetRFChannel(100U);
    NRF_OpenReadingPipe(PIPE0, ModuleAddr, 8U, 1U, 1U);
    NRF_OpenReadingPipe(PIPE1, AddrPipe1, 8U, 0U, 1U);
    NRF_SetTxAddr(ModuleAddr);
    NRF_SetART(10U, 4U);
    NRF_StartListening();
}

void NetworkManager(void) {

    if (NwtFlags.P1_Rx != 0U) {
        CmdPWM1 = DataPipe[1][DEVICE_ADDR * 2U];
        CmdPWM2 = DataPipe[1][(DEVICE_ADDR *2U) + 1U];
        NwtFlags.P1_Rx = 0U;
    }
    else {
        
    }
    
    if (NwtFlags.P0_Rx != 0U) {
        if (DataPipe[0][0] == 0x07U) {
            CmdPWM1 = DataPipe[0][1];
            Slope1 = DataPipe[0][2];
            CmdPWM2 = DataPipe[0][3];
            Slope2 = DataPipe[0][4];
        } else {
        }
        NwtFlags.P0_Rx = 0U;
    } else {
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