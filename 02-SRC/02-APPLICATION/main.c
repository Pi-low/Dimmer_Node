#include "../04-DRIVER/MCU_config.h"
#include <xc.h>
#include <stdint.h>
#include "../01-GLOBALS/common.h"
#include "../03-NETWORK/network.h"
#include "../04-DRIVER/NRF24L01/NRF24L01.h"
#include "../04-DRIVER/MCU_config.h"
#include "../04-DRIVER/target.h"
#include "Appl.h"

void main (void)
{
    uint16_t Task10ms = 10;
    uint16_t Task100ms = 100;
    uint16_t Task1000ms = 1000;
    uint16_t PreviousTick = 0;
    MCU_Init();
    NVM_Init();
    NetworkInit();
    Appl_Init();
    
    while (1)
    {
        Task_LedManager();
        DataTxManager();
        if (PreviousTick != Tick_1ms)
        {
            NetworkManager();
            PreviousTick = Tick_1ms;
        }
        /*=========================*/
        /*        TASK 10MS        */
        /*=========================*/
        if (Tick_1ms >= Task10ms)
        {
            Task10ms = Tick_1ms + 10;
            APPL_TASK_10MS();
        }
        
        /*=========================*/
        /*       TASK 100MS        */
        /*=========================*/
        if (Tick_1ms >= Task100ms)
        {
            Task100ms = Tick_1ms + 100;
            RefreshVoltage(AcquireADCChan(8));
        }
        
        /*=========================*/
        /*         TASK 1S         */
        /*=========================*/
        if (Tick_1ms >= Task1000ms)
        {
            Task1000ms = Tick_1ms + 1000;
            
        }
    }
}