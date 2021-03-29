#include <xc.h>
#include <stdint.h>
#include "../01-GLOBALS/common.h"
#include "Appl.h"
#include "../04-DRIVER/target.h"
#include "../04-DRIVER/NRF24L01/NRF24L01.h"

static void QueueInit(void);
static uint8_t IsFull(void);
static uint8_t IsEmpty(void);
static uint8_t KeepDistance(uint8_t amount);
static void enQueue(uint8_t element);
static uint8_t deQueue(void);
static void LinearTransiant(uint8_t NewValue, uint8_t OldValue, uint8_t Duration10ms);

//const uint8_t gamma[256] = {
//0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
//1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
//2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
//5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
//10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
//17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
//25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
//37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
//51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
//69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
//90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
//115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
//144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
//177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
//215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255};

const uint16_t gamma[256] = {
     0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   2,   2,   2,   3,   3,   4,
     4,   5,   5,   6,   6,   7,   8,   8,   9,  10,  11,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  22,  23,  24,  25,  26,  28,  29,  30,  32,  33,  35,
    36,  38,  39,  41,  43,  44,  46,  48,  49,  51,  53,  55,  57,  59,  60,  62,
    64,  66,  69,  71,  73,  75,  77,  79,  82,  84,  86,  88,  91,  93,  96,  98,
   101, 103, 106, 108, 111, 114, 116, 119, 122, 125, 127, 130, 133, 136, 139, 142,
   145, 148, 151, 154, 157, 160, 164, 167, 170, 173, 177, 180, 184, 187, 190, 194,
   197, 201, 204, 208, 212, 215, 219, 223, 227, 230, 234, 238, 242, 246, 250, 254,
   258, 262, 266, 270, 274, 278, 282, 287, 291, 295, 300, 304, 308, 313, 317, 322,
   326, 331, 335, 340, 345, 349, 354, 359, 363, 368, 373, 378, 383, 388, 393, 398,
   403, 408, 413, 418, 423, 428, 434, 439, 444, 449, 455, 460, 465, 471, 476, 482,
   487, 493, 498, 504, 510, 515, 521, 527, 533, 538, 544, 550, 556, 562, 568, 574,
   580, 586, 592, 598, 604, 611, 617, 623, 629, 636, 642, 648, 655, 661, 668, 674,
   681, 687, 694, 700, 707, 714, 720, 727, 734, 741, 748, 755, 761, 768, 775, 782,
   789, 796, 804, 811, 818, 825, 832, 839, 847, 854, 861, 869, 876, 884, 891, 899,
   906, 914, 921, 929, 937, 944, 952, 960, 968, 975, 983, 991, 999,1007,1015,1023,
  };

static t_Circular_queue MyQueue;
static uint8_t CircularBuffer[QUEUE_BUF_SIZE] = {0};
static uint8_t Click = 0u;
static uint16_t Click_Duration = 0u;
static uint8_t NRF_Payload[8] = {0};

void Appl_Init(void) {
    LED_PIN = 0;
    SetPWM1(0);
    SetPWM2(0);
    QueueInit();
    NRF24L01_Init(SPI_Exchange);
    NRF_SetCRCLen(1);
    NRF_SetAddrWidth(5);
    NRF_SetRFDataRate(NRF_1MBPS);
    NRF_SetRFPower(NRF_PWR_MAX);
    NRF_SetRFChannel(100);
    NRF_OpenReadingPipe(0, "Node0", 8, 1, 1);
    NRF_SetART(10, 4);
    NRF_SetMaskIRQ(NRF_IRQ_RX_DR);
    NRF_StartListening();
}

static void QueueInit(void)
{
    MyQueue.BufferSize = QUEUE_BUF_SIZE - 1u;
    MyQueue.Front = -1;
    MyQueue.Rear = -1;
}

static uint8_t IsFull(void)
{
    return ((MyQueue.Front == (MyQueue.Rear + 1)) || ((MyQueue.Front == 0) && (MyQueue.Rear == (MyQueue.BufferSize - 1)))) ? 1u : 0u;
}

static uint8_t IsEmpty(void)
{
    return (MyQueue.Front == -1) ? 1 : 0;
}

static uint8_t KeepDistance(uint8_t amount)
{
    int16_t diff;
    uint8_t retVal = 1;
    if (MyQueue.Rear > MyQueue.Front)
    {
        diff = MyQueue.Rear - MyQueue.Front;
        if (diff > amount)
        {
            retVal = 0;
        }
        else
        {  }
        
    }
    else
    {  }
    return retVal;
}

static void enQueue(uint8_t element)
{
    if (IsFull() == 0)
    {
        MyQueue.Front += (MyQueue.Front == -1) ? 1 : 0;
        MyQueue.Rear = (MyQueue.Rear + 1) % MyQueue.BufferSize;
        CircularBuffer[MyQueue.Rear] = element;
    }
    else { /* Do Nothing */ }
}

static uint8_t deQueue(void)
{
    uint8_t element = 0;
    if (IsEmpty() == 0)
    {
        element = CircularBuffer[MyQueue.Front];
        if (MyQueue.Front == MyQueue.Rear)
        {
            MyQueue.Front = -1;
            MyQueue.Rear = -1;
        }
        else
        {
            MyQueue.Front = (MyQueue.Front + 1) % MyQueue.BufferSize;
        }
    }
    return element;
}

static void LinearTransiant(uint8_t NewValue, uint8_t OldValue, uint8_t Duration10ms)
{
    uint16_t diff;
    uint8_t a, i;
    float Slope;
    if (NewValue != OldValue)
    {
        if (NewValue > OldValue)
        {
            diff = NewValue - OldValue;
            Slope = (float)diff / Duration10ms;
            for (i = 1; i < Duration10ms; i++)
            {
                a = (uint8_t)(i * Slope) + OldValue;
                enQueue(a);
            }
            enQueue(NewValue);
        }
        else
        {
            diff = OldValue - NewValue;
            Slope = (float)diff / Duration10ms;
            for (i = 1; i < Duration10ms; i++)
            {
                a = OldValue - (uint8_t)(i * Slope);
                enQueue(a);
            }
            enQueue(NewValue);
        }
    }
    else { /* Do Nothing */ }
    
}

void Task_LedManager(void) {
    static uint16_t Timeout = 0;
    if (Click) {
        Click = 0;
        LED_PIN = 1;
        Timeout = Tick_1ms + Click_Duration;
    }
    else if ((Tick_1ms >= Timeout) && (LED_PIN == 1)) {
        LED_PIN = 0;
    }
    else {
        
    }
}

void ClickLed(uint16_t DurationMs) {
    Click = 1;
    Click_Duration = DurationMs;
}

void APPL_TASK_10MS(void) {
    static uint8_t OldValue = 0;
    if (NRF_Available(PIPE0)) {
        ClickLed(100U);
        NRF_ReadPayload(NRF_Payload, 8U);
        if (OldValue != NRF_Payload[0]) {
            LinearTransiant(NRF_Payload[0], OldValue, 50U);
            OldValue = NRF_Payload[0];
        }
        else {
            
        }
    }
    else {
    }
    NRF_StatusHandler();
    if (KeepDistance(1U) != 0U) {
        enQueue(OldValue);
    }
    else {
    }
    SetPWM1(gamma[deQueue()]);
}