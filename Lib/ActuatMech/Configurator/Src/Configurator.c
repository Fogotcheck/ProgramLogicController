#include "Configurator.h"

TaskHandle_t ConfThrHandle = NULL;
QueueHandle_t ConfiauratorQueue = NULL;
static lwjson_stream_parser_t LwJsonStream;
ConfigCh_t Config[ACTUAT_MECH_COUNT] = { 0 };
ConfigCh_t *CurCh = NULL;

static void prv_callback_func(lwjson_stream_parser_t *jsp,
			      lwjson_stream_type_t type);
void ConfSetCh(uint32_t num, ConfigCh_t **ch);
void ConfInterfaceParse(lwjson_stream_parser_t *jsp, lwjson_stream_type_t type,
			ConfigCh_t *Ch);
void ConfDriverParse(lwjson_stream_parser_t *jsp, lwjson_stream_type_t type,
		     ConfigCh_t *Ch);
void ConfPostHandleParse(lwjson_stream_parser_t *jsp, lwjson_stream_type_t type,
			 ConfigCh_t *Ch);

void ConfiguratorThread(void *arg);

int ConfiguratorInit(void)
{
	BaseType_t ret = xTaskCreate(ConfiguratorThread, CONF_MODULE_NAME,
				     CONF_THR_STACK, NULL, CONF_THR_PRIORITIES,
				     &ConfThrHandle);
	return ret == pdPASS ? 0 : -1;
}

void ConfiguratorThread(__attribute__((unused)) void *arg)
{
	ConfiguratorBuf_t Buf = { 0 };
	ConfiauratorQueue = xQueueCreate(10, sizeof(ConfiguratorBuf_t));
	if (ConfiauratorQueue == NULL) {
		ErrMessage();
		Error_Handler();
	}
	if (MechInit()) {
		ErrMessage();
	}

	InfoMessage("Init");

	lwjson_stream_init(&LwJsonStream, prv_callback_func);
	lwjsonr_t res;
	while (1) {
		xQueueReceive(ConfiauratorQueue, &Buf, portMAX_DELAY);
		for (uint16_t i = 0; i < Buf.len; i++) {
			res = lwjson_stream_parse(&LwJsonStream, Buf.data[i]);

			switch (res) {
			case lwjsonSTREAMINPROG:
				break;
			case lwjsonSTREAMWAITFIRSTCHAR:
				InfoMessage("Waiting");
				break;
			case lwjsonSTREAMDONE:
				for (uint8_t j = 0;
				     j < sizeof(Config) / sizeof(Config[0]);
				     j++) {
					if (MechConfigInitStart(Config[j].ch,
								&Config[j])) {
						ErrMessage();
					}
				}
				memset(Config, 0, sizeof(Config));
				CurCh = NULL;
				InfoMessage("Done");
				break;
			default:
				memset(Config, 0, sizeof(Config));
				CurCh = NULL;
				ErrMessage();
				break;
			}
		}
	}
}

static void prv_callback_func(lwjson_stream_parser_t *jsp,
			      lwjson_stream_type_t type)
{
	/* Get a value corresponsing to "k1" key */
	if ((jsp->stack_pos < 2) || (type < LWJSON_STREAM_TYPE_STRING)) {
		return;
	}

	if ((lwjson_stack_seq_5(jsp, 0, OBJECT, KEY, ARRAY, OBJECT, KEY)) &&
	    (type == LWJSON_STREAM_TYPE_NUMBER)) {
		if ((strcmp(jsp->stack[1].meta.name, "ConfCH") == 0) &&
		    (strcmp(jsp->stack[4].meta.name, "NumCH") == 0)) {
			ConfSetCh(atoi(jsp->data.str.buff), &CurCh);
		}
	}
	if (CurCh == NULL) {
		return;
	}

	if (lwjson_stack_seq_3(jsp, 4, KEY, OBJECT, KEY)) {
		ConfInterfaceParse(jsp, type, CurCh);
		ConfDriverParse(jsp, type, CurCh);
		ConfPostHandleParse(jsp, type, CurCh);
	}
}

void ConfSetCh(uint32_t num, ConfigCh_t **ch)
{
	*ch = NULL;
	if (num >= sizeof(Config) / sizeof(Config[0])) {
		return;
	}
	Config[num].ch = num;
	*ch = &Config[num];
}

void ConfInterfaceParse(lwjson_stream_parser_t *jsp, lwjson_stream_type_t type,
			ConfigCh_t *Ch)
{
	if (strcmp(jsp->stack[4].meta.name, "Interface") == 0) {
		if (strcmp(jsp->stack[6].meta.name, "Type") == 0) {
			size_t len = strlen(jsp->data.str.buff);
			if (len >= sizeof(Ch->Interface.type)) {
				return;
			}
			memcpy(Ch->Interface.type, jsp->data.str.buff, len);
		}

		if ((strcmp(jsp->stack[6].meta.name, "Speed") == 0) &&
		    (type == LWJSON_STREAM_TYPE_NUMBER)) {
			Ch->Interface.param[0] = atoi(jsp->data.str.buff);
		}
	}
}

void ConfDriverParse(lwjson_stream_parser_t *jsp, lwjson_stream_type_t type,
		     ConfigCh_t *Ch)
{
	static uint8_t DriverParamCount = 0;
	if (strcmp(jsp->stack[4].meta.name, "Driver") == 0) {
		if (strcmp(jsp->stack[6].meta.name, "Type") == 0) {
			size_t len = strlen(jsp->data.str.buff);
			if (len >= sizeof(Ch->Driver.type)) {
				return;
			}
			memcpy(Ch->Driver.type, jsp->data.str.buff, len);
		}

		if ((strcmp(jsp->stack[6].meta.name, "Param") == 0) &&
		    (type == LWJSON_STREAM_TYPE_NUMBER)) {
			Ch->Driver.param[DriverParamCount++] =
				atoi(jsp->data.str.buff);
		} else {
			DriverParamCount = 0;
		}
	} else {
		DriverParamCount = 0;
	}
}

void ConfPostHandleParse(lwjson_stream_parser_t *jsp, lwjson_stream_type_t type,
			 ConfigCh_t *Ch)
{
	if (strcmp(jsp->stack[4].meta.name, "Posthandle") == 0) {
		if (strcmp(jsp->stack[6].meta.name, "filter") == 0) {
			if (type == LWJSON_STREAM_TYPE_TRUE) {
				Ch->PostHandle.filter = 1;
			}
		}
	}
}