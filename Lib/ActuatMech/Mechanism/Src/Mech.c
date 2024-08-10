#include "Mech.h"

ActuatMech_t MechHandlers[ACTUAT_MECH_COUNT] = { 0 };
extern QueueHandle_t ReportQueue;

void MechThreads(void *arg);
void MechEventHandler(EventBits_t Event, MechPrivateHandleType_t *MechHandle);
uint32_t MechCheckConfig(ConfigCh_t *Config);
void MechSetBuf(MechPrivateBuf_t *Mem, MechPrivateBuf_t **Buf);
int MechInterfaceInit(MechPrivateHandleType_t *Mech);
int MechDriverInit(MechPrivateHandleType_t *Mech);
void MechDeinit(MechPrivateHandleType_t *Mech);
int MechInitEventHandler(MechPrivateHandleType_t *Mech);

int MechStartEventHandler(MechPrivateHandleType_t *Mech);

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
	MechPrivateHandleType_t Mech = { 0 };
	Mech.ThrNum = (uint32_t)arg;
	MechHandlers[Mech.ThrNum].EventHandle = xEventGroupCreate();
	if (MechHandlers[Mech.ThrNum].EventHandle == NULL) {
		ErrMessage("0x%x", Mech.ThrNum);
	}
	MechHandlers[Mech.ThrNum].QueueHandle =
		xQueueCreate(1, sizeof(MechHandlers[Mech.ThrNum].ChHandle));
	if (MechHandlers[Mech.ThrNum].QueueHandle == NULL) {
		ErrMessage("[%d]", Mech.ThrNum);
	}

	MechPrivateBuf_t Buf = { 0 };
	MechSetBuf(&Buf, &Mech.Buf);
	InfoMessage("Init[%d]", Mech.ThrNum);

	EventBits_t Event = 0;
	EventBits_t Mask = 1;
	while (1) {
		Event = xEventGroupWaitBits(
			MechHandlers[Mech.ThrNum].EventHandle, MECH_ALL_EVENTS,
			pdFALSE, pdFALSE, portMAX_DELAY);
		Mask = 1;
		for (uint8_t i = 0; i < configUSE_16_BIT_TICKS; i++) {
			if (Event & Mask) {
				MechEventHandler(Event & Mask, &Mech);
			}
			Mask <<= 1;
		}
	}
}

void MechEventHandler(EventBits_t Event, MechPrivateHandleType_t *Mech)
{
	xEventGroupClearBits(MechHandlers[Mech->ThrNum].EventHandle, Event);
	InfoMessage("event[%d]::0x%x", Mech->ThrNum, Event);

	char ChSuffix[ACTUAT_MECH_SUFFIX_SIZE] = { 0 };
	strcat(&ChSuffix[0], "/ch[");
	itoa(Mech->ThrNum, &ChSuffix[4], 10);
	strcat(ChSuffix, "]/");

	switch (Event) {
	case MECH_INIT: {
		MechDeinit(Mech);
		if (MechInitEventHandler(Mech)) {
			ErrMessage("[%d]", Mech->ThrNum);
			break;
		}

		ActuatMechReport_t InitReport = { 0 };
		strcat(InitReport.Suffix, ChSuffix);
		strcat(InitReport.Suffix, Mech->Interface->type);
		for (uint16_t i = 0; i < ACTUAT_MECH_TYPE_PARAM_SIZE; i++) {
			char TmpData[ACTUAT_MECH_DATA_SIZE] = "0x";
			itoa(MechHandlers[Mech->ThrNum]
				     .ChHandle.Interface.param[i],
			     &TmpData[2], 16);
			strcat(InitReport.Data, TmpData);
			strcat(InitReport.Data, ";");
		}
		if (xQueueSend(ReportQueue, &InitReport, 10) != pdTRUE) {
			vTaskDelay(100);
			if (xQueueSend(ReportQueue, &InitReport, 10) !=
			    pdTRUE) {
				WarningMessage("[%d]", Mech->ThrNum);
				break;
			}
		}
		memset(&InitReport, 0, sizeof(InitReport));
		strcat(InitReport.Suffix, ChSuffix);
		strcat(InitReport.Suffix, Mech->Driver->type);
		for (uint16_t i = 0; i < ACTUAT_MECH_TYPE_PARAM_SIZE; i++) {
			char TmpData[ACTUAT_MECH_DATA_SIZE] = "0x";
			itoa(MechHandlers[Mech->ThrNum].ChHandle.Driver.param[i],
			     &TmpData[2], 16);
			strcat(InitReport.Data, TmpData);
			strcat(InitReport.Data, ";");
		}
		if (xQueueSend(ReportQueue, &InitReport, 10) != pdTRUE) {
			vTaskDelay(100);
			if (xQueueSend(ReportQueue, &InitReport, 10) !=
			    pdTRUE) {
				WarningMessage("[%d]", Mech->ThrNum);
				break;
			}
		}
		xEventGroupSetBits(MqttClientEvent, MQTT_SEND_REPORT);
		xEventGroupSetBits(MechHandlers[Mech->ThrNum].EventHandle,
				   MECH_START);
		break;
	}
	case MECH_START: {
		if (MechStartEventHandler(Mech)) {
			ErrMessage("[%d]", Mech->ThrNum);
			break;
		}
		break;
	}
	default:
		break;
	}
}

int MechStartEventHandler(MechPrivateHandleType_t *Mech)
{
	int state = 0;
	if (Mech->Driver->Request == NULL) {
		ErrMessage("[%d]", Mech->ThrNum);
		return -1;
	}
	state = Mech->Driver->Request(
		Mech->Interface,
		MechHandlers[Mech->ThrNum].ChHandle.Driver.param,
		Mech->Buf->Raw);
	if (state) {
		ErrMessage("[%d]", Mech->ThrNum);
		return -1;
	}

	return 0;
}

void MechDeinit(MechPrivateHandleType_t *Mech)
{
	Mech->Driver = NULL;
	Mech->Interface = NULL;
}

int MechInitEventHandler(MechPrivateHandleType_t *Mech)
{
	xQueueReceive(MechHandlers[Mech->ThrNum].QueueHandle,
		      &MechHandlers[Mech->ThrNum].ChHandle, 0);
	DrivCompGetInterface(
		&Mech->Interface,
		MechHandlers[Mech->ThrNum].ChHandle.Interface.type);
	DrivCompGetDriver(&Mech->Driver,
			  MechHandlers[Mech->ThrNum].ChHandle.Driver.type);
	if ((Mech->Driver == NULL) || (Mech->Interface == NULL)) {
		ErrMessage("[%d]", Mech->ThrNum);
		return -1;
	}

	if (MechInterfaceInit(Mech)) {
		ErrMessage("[%d]", Mech->ThrNum);
		return -1;
	}
	if (MechDriverInit(Mech)) {
		ErrMessage("[%d]", Mech->ThrNum);
		return -1;
	}

	return 0;
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
		ErrMessage("[%d]", Ch);
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

inline int MechInterfaceInit(MechPrivateHandleType_t *Mech)
{
	int state = 0;
	if (MechHandlers[Mech->ThrNum].ChHandle.Interface.param[0] == 0) {
		if (Mech->Interface->SetDefault != NULL) {
			state = Mech->Interface->SetDefault(
				MechHandlers[Mech->ThrNum]
					.ChHandle.Interface.type,
				MechHandlers[Mech->ThrNum]
					.ChHandle.Interface.param);
			if (state) {
				ErrMessage("[%d]", Mech->ThrNum);
				return -1;
			}
		}
	}
	if (Mech->Interface->Init == NULL) {
		ErrMessage("[%d]", Mech->ThrNum);
		return -1;
	}

	state = Mech->Interface->Init(
		Mech->Interface->Handle,
		MechHandlers[Mech->ThrNum].ChHandle.Interface.param);
	if (state) {
		ErrMessage("[%d]", Mech->ThrNum);
		return -1;
	}
	return 0;
}

inline int MechDriverInit(MechPrivateHandleType_t *Mech)
{
	int state = 0;
	if (MechHandlers[Mech->ThrNum].ChHandle.Driver.param[0] == 0) {
		if (Mech->Driver->SetDefault != NULL) {
			state = Mech->Driver->SetDefault(
				Mech->Interface,
				MechHandlers[Mech->ThrNum]
					.ChHandle.Driver.param);
			if (state) {
				ErrMessage("[%d]", Mech->ThrNum);
				return -1;
			}
		}
	}
	if (Mech->Driver->Init == NULL) {
		ErrMessage("[%d]", Mech->ThrNum);
		return -1;
	}

	state = Mech->Driver->Init(
		Mech->Interface,
		MechHandlers[Mech->ThrNum].ChHandle.Driver.param);
	if (state) {
		ErrMessage("[%d]", Mech->ThrNum);
		return -1;
	}
	return 0;
}

void MechSetBuf(MechPrivateBuf_t *Mem, MechPrivateBuf_t **Buf)
{
	*Buf = Mem;
}