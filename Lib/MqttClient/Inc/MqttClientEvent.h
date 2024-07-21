#ifndef __MqttClientEvent_h__
#define __MqttClientEvent_h__

#include "FreeRTOS.h"
#include "event_groups.h"

extern EventGroupHandle_t MqttClientEvent;

enum MqttClientEventEnum {
	MQTT_CREATE_CLIENT = (EventBits_t)(1 << 0),
	MQTT_FREE_CLIENT = (EventBits_t)(1 << 1),
    
	MQTT_ALL_EVENTS = (EventBits_t)(MQTT_CREATE_CLIENT) |
			  (MQTT_FREE_CLIENT),
};

#endif //__MqttClientEvent_h__