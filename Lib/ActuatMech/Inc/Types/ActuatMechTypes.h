#ifndef __ActuatMechTypes_h__
#define __ActuatMechTypes_h__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

#include "ActuatMechDef.h"

typedef struct ConfInterface {
	char type[ACTUAT_MECH_TYPE_NAME_SIZE];
	uint32_t param[ACTUAT_MECH_TYPE_PARAM_SIZE];
} ConfInterface_t;

typedef struct ConfDriver {
	char type[ACTUAT_MECH_TYPE_NAME_SIZE];
	uint32_t param[ACTUAT_MECH_TYPE_PARAM_SIZE];
} ConfDriver_t;

typedef struct ConfPostHandle {
	uint8_t filter : 1;
	uint8_t revers : 1;
	uint8_t fourier : 1;
} ConfPostHandle_t;

typedef struct ConfCh {
	uint32_t ch;
	ConfInterface_t Interface;
	ConfDriver_t Driver;
	ConfPostHandle_t PostHandle;
} ConfigCh_t;

typedef struct ActuatMech {
	TaskHandle_t ThrHandle;
	QueueHandle_t QueueHandle;
	EventGroupHandle_t EventHandle;
	TimerHandle_t TimerHandle;
	SemaphoreHandle_t SemphHandle;
	ConfigCh_t ChHandle;
} ActuatMech_t;

typedef struct ActuatMechReport {
	char Suffix[ACTUAT_MECH_SUFFIX_SIZE];
	char Data[ACTUAT_MECH_DATA_SIZE];
} ActuatMechReport_t;

#endif //__ActuatMechTypes_h__