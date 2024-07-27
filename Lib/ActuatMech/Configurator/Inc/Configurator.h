#ifndef __Configurator_h__
#define __Configurator_h__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "lwip/apps/mqtt.h"
#include "lwjson/lwjson.h"
#include "main.h"
#include "DLog.h"
#include "ActuatMechTypes.h"

#define CONF_MODULE_NAME "ConfTask"
#define CONF_THR_STACK 1024
#define CONF_THR_PRIORITIES (configMAX_PRIORITIES - 10)

typedef struct ConfiguratorBuf
{
    uint16_t len;
    uint8_t flag;
    uint8_t data[MQTT_VAR_HEADER_BUFFER_LEN];
}ConfiguratorBuf_t;


int ConfiguratorInit(void);

#endif //__Configurator_h__