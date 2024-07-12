#ifndef __Led_h__
#define __Led_h__

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#define LED_MODULE_NAME "LedTask"
#define LED_THR_STACK configMINIMAL_STACK_SIZE
#define LED_THR_PRIORITIES 1
#define LED_TIMER_NAME "LedTimer"

enum LedTypeEnum { LED_GREEN, LED_LINK, LED_ERR, LED_COUNT };

typedef struct LedBuf
{
    uint8_t ch;
    uint8_t state;
	uint16_t period;
}LedHandle_t;

enum TimerState
{
    LED_STOP,
    LED_START,
};

typedef struct LedTimerType {
    LedHandle_t Handle;
	TimerHandle_t xTimer;
} LedTimerType_t;

int LedInit(void);
int LedStart(uint8_t ch, uint16_t period);
int LedStop(uint8_t ch);

#endif //__Led_h__