/* 
 * File:   Appl.h
 * Author: Nello
 *
 * Created on 29 mars 2021, 21:37
 */

#ifndef APPL_H
#define	APPL_H

#include <xc.h>
#include <stdint.h>

#define QUEUE_BUF_SIZE 256U
#define PIPE0 0U
#define PIPE1 1U
#define PIPE2 2U
#define PIPE3 3U
#define PIPE4 4U
#define PIPE5 5U

typedef struct
{
    int16_t Front;
    int16_t Rear;
    uint16_t BufferSize;
}t_Circular_queue;

void Appl_Init(void);
void Task_LedManager(void);
void ClickLed(uint16_t DurationMs);
void APPL_TASK_10MS(void);
#endif	/* APPL_H */

