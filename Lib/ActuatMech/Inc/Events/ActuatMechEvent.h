#ifndef __ActuatMechEvent_h__
#define __ActuatMechEvent_h__

#include "FreeRTOS.h"
#include "event_groups.h"
#include "ActuatMechTypes.h"

extern ActuatMech_t MechHandlers[];

enum ActuatMechEvent {
	MECH_INIT = (EventBits_t)(1 << 0),
	MECH_START = (EventBits_t)(1 << 1),
	MECH_RAW_DATA = (EventBits_t)(1 << 2),
	MECH_WAIT = (EventBits_t)(1 << 3),
	MECH_ALL_EVENTS = (EventBits_t)(MECH_INIT) | (MECH_START) |
			  (MECH_RAW_DATA) | (MECH_WAIT)
};

#endif //__ActuatMechEvent_h__