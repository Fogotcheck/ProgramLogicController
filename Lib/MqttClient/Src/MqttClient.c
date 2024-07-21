#include "MqttClient.h"

TaskHandle_t MqttThrHandle = NULL;
TimerHandle_t MqttTimerControl = NULL;
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
static char TopicPrefix[64] = { 0 };
static uint8_t TimerControlState = MQTT_TIMER_CHECK_LINK;

void MqttThread(void *arg);
void MqttEventsHandler(EventBits_t Event);

static void mqtt_incoming_publish_cb(void *arg, const char *topic,
				     u32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
				  u8_t flags);
static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
			       mqtt_connection_status_t status);
static void mqtt_request_cb(void *arg, err_t err);

void MqttTimerCb(TimerHandle_t xTimer);

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
	MqttTimerControl = xTimerCreate("MqttTimer",
					pdMS_TO_TICKS(MQTT_TIMER_TICK_MS),
					pdTRUE, NULL, MqttTimerCb);
	if (MqttTimerControl == NULL) {
		ErrMessage();
	}

	/* todo: add find server */
	ipaddr_aton("192.168.0.1", &mqtt_ip);
	strcat(TopicPrefix, "plk/");
	strcat(TopicPrefix, ipaddr_ntoa(&ipaddr));

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
				MqttEventsHandler(Event & Mask);
			}
			Mask <<= 1;
		}
	}
}

void MqttEventsHandler(EventBits_t Event)
{
	xEventGroupClearBits(MqttClientEvent, Event);
	InfoMessage("event::0x%x", Event);
	static uint8_t SwitchTimerControl = 0;
	switch (Event) {
	case MQTT_TRY_CONNECT:
		if (SwitchTimerControl == 0) {
			SwitchTimerControl = 1;
			xTimerStart(MqttTimerControl, 0);
		}
		__attribute__((unused)) err_t ret = mqtt_client_connect(
			mqtt_client, &mqtt_ip, MQTT_PORT, mqtt_connection_cb,
			LWIP_CONST_CAST(void *, &mqtt_client_info),
			&mqtt_client_info);
		break;
	case MQTT_DISCONNECT:
		if (SwitchTimerControl == 1) {
			SwitchTimerControl = 0;
			xTimerStop(MqttTimerControl, 0);
		}
		mqtt_disconnect(mqtt_client);
		break;
	case MQTT_LINK_CONNECT: {
		char TopicName[128] = { 0 };
		strcat(TopicName, TopicPrefix);
		strcat(TopicName, "/conf");
		mqtt_publish(mqtt_client, TopicName, "Wait config", 11, 0, 0,
			     NULL, NULL);
		mqtt_subscribe(mqtt_client, TopicName, 1, mqtt_request_cb,
			       NULL);
		break;
	}
	default:
		break;
	}
}

void MqttClientStart(void)
{
	xEventGroupSetBits(MqttClientEvent, MQTT_TRY_CONNECT);
}

void MqttClientStop(void)
{
	xEventGroupSetBits(MqttClientEvent, MQTT_DISCONNECT);
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic,
				     u32_t tot_len)
{
	__attribute__((unused))
	const struct mqtt_connect_client_info_t *client_info =
		(const struct mqtt_connect_client_info_t *)arg;

	InfoMessage("%s\t%d", topic, (int)tot_len);
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len,
				  u8_t flags)
{
	__attribute__((unused))
	const struct mqtt_connect_client_info_t *client_info =
		(const struct mqtt_connect_client_info_t *)arg;
	LWIP_UNUSED_ARG(data);

	InfoMessage("%d\t%d", (int)len, (int)flags);
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg,
			       mqtt_connection_status_t status)
{
	__attribute__((unused))
	const struct mqtt_connect_client_info_t *client_info =
		(const struct mqtt_connect_client_info_t *)arg;
	LWIP_UNUSED_ARG(client);
	InfoMessage("status %d", (int)status);

	if (status == MQTT_CONNECT_ACCEPTED) {
		TimerControlState = 0;
		xEventGroupSetBits(MqttClientEvent, MQTT_LINK_CONNECT);

	} else {
		TimerControlState = 1;

		xEventGroupSetBits(MainEvent, MQTT_CRITICAL_ERR);
	}
}

static void mqtt_request_cb(__attribute__((unused)) void *arg, err_t err)
{
	InfoMessage("err %d", (int)err);
	if (err != ERR_OK) {
		xEventGroupSetBits(MqttClientEvent, MQTT_LINK_CONNECT);
		return;
	}
	xEventGroupSetBits(MainEvent, MQTT_WAIT_CONF);
}

void MqttTimerCb(TimerHandle_t xTimer)
{
	if (xTimer == MqttTimerControl) {
		switch (TimerControlState) {
		case MQTT_TIMER_CHECK_LINK:
			if (mqtt_client_is_connected(mqtt_client)) {
				return;
			}
			xEventGroupSetBits(MainEvent, MQTT_CRITICAL_ERR);
			TimerControlState = MQTT_TIMER_REQUEST_CONNECT;
			break;
		case MQTT_TIMER_REQUEST_CONNECT:
			mqtt_disconnect(mqtt_client);
			xEventGroupSetBits(MqttClientEvent, MQTT_TRY_CONNECT);
			break;
		default:
			break;
		}
	}
}