#include "Mech.h"

ActuatMech_t MechHandlers[ACTUAT_MECH_COUNT] = { 0 };

void MechThreads(void *arg);
void MechEventHandler(EventBits_t Event, uint32_t NumThr);
uint32_t MechCheckConfig(ConfigCh_t *Config);

int MechInit(void)
{
	int ret = 0;
	for (uint32_t i = 0; i < sizeof(MechHandlers) / sizeof(MechHandlers[0]);
	     i++) {
		if ((xTaskCreate(MechThreads, MECH_MODULE_NAME, MECH_THR_STACK,
				 (void *)i, MECH_THR_PRIORITIES,
				 &MechHandlers[i].ThrHandle)) != pdPASS) {
			ret = -1;
		}
		vTaskDelay(10);
	}

	return ret;
}

void MechThreads(void *arg)
{
	uint32_t ThrNum = (uint32_t)arg;
	MechHandlers[ThrNum].EventHandle = xEventGroupCreate();
	if (MechHandlers[ThrNum].EventHandle == NULL) {
		ErrMessage();
	}
	MechHandlers[ThrNum].QueueHandle =
		xQueueCreate(1, sizeof(MechHandlers[ThrNum].ChHandle));
	if (MechHandlers[ThrNum].QueueHandle == NULL) {
		ErrMessage();
	}

	InfoMessage("Init::0x%x", ThrNum);

	EventBits_t Event = 0;
	EventBits_t Mask = 1;
	while (1) {
		Event = xEventGroupWaitBits(MechHandlers[ThrNum].EventHandle,
					    MECH_ALL_EVENTS, pdFALSE, pdFALSE,
					    portMAX_DELAY);
		Mask = 1;
		for (uint8_t i = 0; i < configUSE_16_BIT_TICKS; i++) {
			if (Event & Mask) {
				MechEventHandler(Event & Mask, ThrNum);
			}
			Mask <<= 1;
		}
	}
}

void MechEventHandler(EventBits_t Event, uint32_t ThrNum)
{
	xEventGroupClearBits(MechHandlers[ThrNum].EventHandle, Event);
	InfoMessage("event[%d]::0x%x", ThrNum, Event);
	switch (Event) {
	case MECH_INIT:
		/*
        todo stop interface
        stop drivers
        */
		xQueueReceive(MechHandlers[ThrNum].QueueHandle,
			      &MechHandlers[ThrNum].ChHandle, 0);

		break;

	default:
		break;
	}
}

int MechConfigInitStart(uint32_t Ch, ConfigCh_t *Config)
{
	if (Ch >= sizeof(MechHandlers) / sizeof(MechHandlers[0])) {
		return -1;
	}
	
	switch (MechCheckConfig(Config)) {
	case 0:
		/* OK */
		break;
	case 3:
		/* NULL CONF - OK */
		return 0;
	default:
		/* Err */
		return -1;
	}

	if ((MechHandlers[Ch].QueueHandle == NULL) ||
	    (MechHandlers[Ch].EventHandle == NULL)) {
		return -1;
	}

	xQueueSend(MechHandlers[Ch].QueueHandle, Config, 10);
	xEventGroupSetBits(MechHandlers[Ch].EventHandle, MECH_INIT);
	return 0;
}

uint32_t MechCheckConfig(ConfigCh_t *Config)
{
	uint32_t ret = 0;
	if (strlen(Config->Interface.type) == 0) {
		ret |= 1;
	}
	if (strlen(Config->Driver.type) == 0) {
		ret |= 2;
	}
	return ret;
}