#ifndef __MainEvent_h__
#define __MainEvent_h__

#include "FreeRTOS.h"
#include "event_groups.h"

extern EventGroupHandle_t MainEvent;

enum MainEvents {
	ETH_LINK_UP = (EventBits_t)(1 << 0),
	ETH_LINK_DOWN = (EventBits_t)(1 << 1),

	MAIN_ALL_EVENTS = (EventBits_t)(ETH_LINK_UP)|(ETH_LINK_DOWN)
};

#endif //__MainEvent_h__