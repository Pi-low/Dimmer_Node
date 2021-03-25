/* 
 * File:   target.h
 * Author: Nello
 *
 * Created on March 21, 2021, 10:16 PM
 */

#ifndef TARGET_H
#define	TARGET_H

#include <xc.h>

#define CE_LAT LATC0_bit
#define CS__LAT LATC1_bit

void MCU_Init(void);

#endif	/* TARGET_H */

