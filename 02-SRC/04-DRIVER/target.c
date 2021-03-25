#include "../01-GLOBALS/common.h"
#include "target.h"

void MCU_Init(void) {
    OSCCON = 0x78u;
    TRISA = 0xFFu;
    TRISB = 0x3Fu;
    TRISC = 0x44u;
}
