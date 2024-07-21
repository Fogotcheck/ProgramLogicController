#include "MqttClient.h"

TaskHandle_t MqttThrHandle = NULL;
EventGroupHandle_t MqttClientEvent = NULL;
static mqtt_client_t *mqtt_client;
static struct mqtt_connect_client_info_t mqtt_client_info = {
	"test",
	"stm", /* user */
	"root", /* pass */
	100, /* keep alive */
	NULL, /* will_topic */
	NULL, /* will_msg */
	0, /* will_msg_len */
	0, /* will_qos */
	0 /* will_retain */
#if LWIP_ALTCP && LWIP_ALTCP_TLS
	,
	NULL
#endif
};
/* todo */
extern ip4_addr_t ipaddr;
static ip_addr_t mqtt_ip = { 0 };

void MqttThread(void *arg);
void MqttClienEventsHandler(EventBits_t Event);

static void mqtt_incoming_publish_cb(void *arg, const char *topic,
				     u32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
				  u8_t flags);
static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
			       mqtt_connection_status_t status);
static void mqtt_request_cb(void *arg, err_t err);

int MqttClientInit(void)
{
	BaseType_t ret = xTaskCreate(MqttThread, MQTT_MODULE_NAME,
				     MQTT_THR_STACK, NULL, MQTT_THR_PRIORITIES,
				     &MqttThrHandle);
	vTaskDelay(10);
	return ret == pdPASS ? 0 : -1;
}

void MqttThread(__attribute__((unused)) void *arg)
{
	MqttClientEvent = xEventGroupCreate();
	if (MqttClientEvent == NULL) {
		ErrMessage();
	}
	ipaddr_aton("192.168.0.1", &mqtt_ip);
	mqtt_client = mqtt_client_new();
	mqtt_set_inpub_callback(mqtt_client, mqtt_incoming_publish_cb,
				mqtt_incoming_data_cb,
				LWIP_CONST_CAST(void *, &mqtt_client_info));

	EventBits_t Event = 0;
	EventBits_t Mask = 1;
	while (1) {
		Event = xEventGroupWaitBits(MqttClientEvent, MQTT_ALL_EVENTS,
					    pdFALSE, pdFALSE, portMAX_DELAY);
		Mask = 1;
		for (uint8_t i = 0; i < configUSE_16_BIT_TICKS; i++) {
			if (Event & Mask) {
				MqttClienEventsHandler(Event & Mask);
			}
			Mask <<= 1;
		}
	}
}

void MqttClienEventsHandler(EventBits_t Event)
{
	xEventGroupClearBits(MqttClientEvent, Event);
	InfoMessage("event::0x%x", Event);

	switch (Event) {
	case MQTT_CREATE_CLIENT:

		mqtt_client_connect(mqtt_client, &mqtt_ip, MQTT_PORT,
				    mqtt_connection_cb,
				    LWIP_CONST_CAST(void *, &mqtt_client_info),
				    &mqtt_client_info);

		break;
	case MQTT_FREE_CLIENT:

		mqtt_disconnect(mqtt_client);

		break;
	default:
		break;
	}
}

void MqttClientStart(void)
{
	xEventGroupSetBits(MqttClientEvent, MQTT_CREATE_CLIENT);
}

void MqttClientStop(void)
{
	xEventGroupSetBits(MqttClientEvent, MQTT_FREE_CLIENT);
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic,
				     u32_t tot_len)
{
	const struct mqtt_connect_client_info_t *client_info =
		(const struct mqtt_connect_client_info_t *)arg;

	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" publish cb: topic %s, len %d\n",
			    client_info->client_id, topic, (int)tot_len));
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
				  u8_t flags)
{
	const struct mqtt_connect_client_info_t *client_info =
		(const struct mqtt_connect_client_info_t *)arg;
	LWIP_UNUSED_ARG(data);

	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" data cb: len %d, flags %d\n",
			    client_info->client_id, (int)len, (int)flags));
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
			       mqtt_connection_status_t status)
{
	const struct mqtt_connect_client_info_t *client_info =
		(const struct mqtt_connect_client_info_t *)arg;
	LWIP_UNUSED_ARG(client);

	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" connection cb: status %d\n",
			    client_info->client_id, (int)status));

	if (status == MQTT_CONNECT_ACCEPTED) {
		mqtt_sub_unsub(client, "topic_qos1", 1, mqtt_request_cb,
			       LWIP_CONST_CAST(void *, client_info), 1);
		mqtt_sub_unsub(client, "topic_qos0", 0, mqtt_request_cb,
			       LWIP_CONST_CAST(void *, client_info), 1);
	}
}

static void mqtt_request_cb(void *arg, err_t err)
{
	const struct mqtt_connect_client_info_t *client_info =
		(const struct mqtt_connect_client_info_t *)arg;

	LWIP_PLATFORM_DIAG(("MQTT client \"%s\" request cb: err %d\n",
			    client_info->client_id, (int)err));
}
