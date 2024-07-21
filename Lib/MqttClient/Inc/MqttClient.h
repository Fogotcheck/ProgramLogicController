#ifndef __MqttClient_h__
#define __MqttClient_h__

#include "FreeRTOS.h"
#include "main.h"
#include "task.h"
#include "lwip.h"

#include "MqttClientEvent.h"
#include "MainEvent.h"
#include "DLog.h"

#include "lwip/apps/mqtt.h"

#define MQTT_MODULE_NAME "MqttTask"
#define MQTT_THR_STACK 512
#define MQTT_THR_PRIORITIES (configMAX_PRIORITIES - 3)

int MqttClientInit(void);
void MqttClientStart(void);
void MqttClientStop(void);

#endif //__MqttClient_h__