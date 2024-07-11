#ifndef __FreeRtosCustom_h__
#define __FreeRtosCustom_h__

#include "FreeRTOS.h" // ARM.FreeRTOS::RTOS:Core
#include "task.h"

/* Callback function prototypes */
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void vApplicationDaemonTaskStartupHook(void);
extern void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                          signed char *pcTaskName);

/* External Idle and Timer task static memory allocation functions */
extern void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                          StackType_t **ppxIdleTaskStackBuffer,
                                          uint32_t *pulIdleTaskStackSize);
extern void
vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                               StackType_t **ppxTimerTaskStackBuffer,
                               uint32_t *pulTimerTaskStackSize);

#endif //__FreeRtosCustom_h__