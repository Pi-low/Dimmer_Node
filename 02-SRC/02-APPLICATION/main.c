#include "../04-DRIVER/MCU_config.h"
#include <xc.h>
#include <stdint.h>
#include "../01-GLOBALS/common.h"
#include "../04-DRIVER/NRF24L01/NRF24L01.h"
#include "../04-DRIVER/MCU_config.h"
#include "main.h"
#include "../04-DRIVER/target.h"

void main (void) {
    uint16_t timeout = 200u;
    uint8_t P0ddr[5] = {0u};
    MCU_Init();
    NRF24L01_Init(SPI_Exchange);
    LED_PIN = 0u;
    NRF_Read_Register(REG_NRF_TX_ADDR, P0ddr, 5u);
    
    while (1u) {
        if (Tick_1ms >= timeout) {
            LED_PIN ^= 1u;
            timeout = Tick_1ms + 200u;
        }
    }
}