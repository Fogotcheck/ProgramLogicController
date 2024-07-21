#ifndef __MainEvent_h__
#define __MainEvent_h__

#include "FreeRTOS.h"
#include "event_groups.h"

extern EventGroupHandle_t MainEvent;

enum MainEventsEnum {
	ETH_LINK_UP = (EventBits_t)(1 << 0),
	ETH_LINK_DOWN = (EventBits_t)(1 << 1),
	MQTT_WAIT_CONF = (EventBits_t)(1 << 2),
	MQTT_CRITICAL_ERR = (EventBits_t)(1 << 3),

	MAIN_ALL_EVENTS = (EventBits_t)(ETH_LINK_UP) | (ETH_LINK_DOWN) |
			  (MQTT_WAIT_CONF) | (MQTT_CRITICAL_ERR)
};

typedef struct MainEventErrBit {
	uint8_t mqtt : 1;

} MainEventErrBit_t;

typedef union MainEventErrUnion {
	MainEventErrBit_t Bit;
	uint32_t u32;
} MainEventErrUnion_t;

#endif //__MainEvent_h__