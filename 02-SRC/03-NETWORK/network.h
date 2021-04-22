/* 
 * File:   network.h
 * Author: Nello
 *
 * Created on 5 avril 2021, 09:38
 */

#ifndef NETWORK_H
#define	NETWORK_H

typedef union {
    uint8_t Byte;
    struct {
        uint8_t P0_Rx : 1;
        uint8_t P1_Rx : 1;
        uint8_t P2_Rx : 1;
        uint8_t P3_Rx : 1;
        uint8_t P4_Rx : 1;
        uint8_t P5_Rx : 1;
        uint8_t Tx_Dat : 1;
    };
}u_Flags;

extern uint8_t DataPipe[2][8];
extern uint8_t DataTx[8];
extern u_Flags NwtFlags;

void NetworkInit(void);
void NetworkManager(void);
uint8_t Get_PWM1Cmd(void);
uint8_t Get_PWM2Cmd(void);
uint8_t Get_Slope1(void);
uint8_t Get_Slope2(void);

#endif	/* NETWORK_H */

