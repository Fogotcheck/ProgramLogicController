#ifndef __MqttClientEvent_h__
#define __MqttClientEvent_h__

#include "FreeRTOS.h"
#include "event_groups.h"

extern EventGroupHandle_t MqttClientEvent;

enum MqttClientEventEnum {
	MQTT_TRY_CONNECT = (EventBits_t)(1 << 0),
	MQTT_DISCONNECT = (EventBits_t)(1 << 1),
	MQTT_LINK_CONNECT = (EventBits_t)(1 << 2),
	MQTT_SEND_REPORT = (EventBits_t)(1 << 3),
	MQTT_ALL_EVENTS = (EventBits_t)(MQTT_TRY_CONNECT) | (MQTT_DISCONNECT) |
			  (MQTT_LINK_CONNECT) | (MQTT_SEND_REPORT),
};

#endif //__MqttClientEvent_h__