#include "../04-DRIVER/MCU_config.h"
#include <xc.h>
#include <stdint.h>
#include "../01-GLOBALS/common.h"
#include "../04-DRIVER/NRF24L01/NRF24L01.h"
#include "../04-DRIVER/MCU_config.h"
#include "Appl.h"
#include "../04-DRIVER/target.h"

void main (void) {
    uint16_t Task10ms = 10U;
    uint16_t Task100ms = 100U;
    uint16_t Task1000ms = 1000U;
    MCU_Init();
    Appl_Init();
    
    while (1U) {
        Task_LedManager();
        
        /*=========================*/
        /*        TASK 10MS        */
        /*=========================*/
        if (Tick_1ms >= Task10ms) {
            Task10ms = Tick_1ms + 10U;
            
            APPL_TASK_10MS();
        }
        else {
        }
        
        /*=========================*/
        /*       TASK 100MS        */
        /*=========================*/
        if (Tick_1ms >= Task100ms) {
            Task100ms = Tick_1ms + 100U;
            
        }
        else {
        }
        
        /*=========================*/
        /*         TASK 1S         */
        /*=========================*/
        if (Tick_1ms >= Task1000ms) {
            Task1000ms = Tick_1ms + 1000U;
            
        }
        else {
        }
    }
}