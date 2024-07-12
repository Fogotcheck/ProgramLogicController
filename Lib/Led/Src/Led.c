#include "Led.h"

TaskHandle_t LedThrHandle = NULL;
QueueHandle_t LedQueueHandle = NULL;
TimerHandle_t LedTimerHandle[LED_COUNT] = { 0 };

void LedThread(void *arg);
void LedTimerCallback(TimerHandle_t xTimer);
void LedSwitchPin(uint8_t ch, uint8_t state);

int LedInit(void)
{
	BaseType_t ret = xTaskCreate(LedThread, LED_MODULE_NAME, LED_THR_STACK,
				     NULL, LED_THR_PRIORITIES, &LedThrHandle);
	return ret == pdPASS ? 0 : -1;
}

void LedThread(__attribute__((unused)) void *arg)
{
	for (uint8_t i = 0;
	     i < (sizeof(LedTimerHandle) / sizeof(LedTimerHandle[0])); i++) {
		LedTimerHandle[i] = xTimerCreate(LED_TIMER_NAME, portMAX_DELAY,
						 pdTRUE, NULL,
						 LedTimerCallback);
		if (LedTimerHandle[i] == NULL) {
			Error_Handler();
		}
	}

	LedQueueHandle = xQueueCreate((LED_COUNT + 1), sizeof(LedHandle_t));
	if (LedQueueHandle == NULL) {
		Error_Handler();
	}

	LedHandle_t buf = { 0 };
	while (1) {
		xQueueReceive(LedQueueHandle, &buf, portMAX_DELAY);
		if (buf.ch >= LED_COUNT) {
			continue;
		}
		if (buf.period >= 50) {
			xTimerChangePeriod(LedTimerHandle[buf.ch],
					   pdMS_TO_TICKS(buf.period), 0);
		} else {
			xTimerStop(LedTimerHandle[buf.ch], 5);
			LedSwitchPin(buf.ch, buf.state);
			continue;
		}

		switch (buf.state) {
		case LED_STOP:
			xTimerStop(LedTimerHandle[buf.ch], 5);
			LedSwitchPin(buf.ch, buf.state);
			break;
		case LED_START:
			xTimerStart(LedTimerHandle[buf.ch], 5);
			LedSwitchPin(buf.ch, buf.state);
			break;
		default:
			break;
		}
	}
}

void LedSwitchPin(uint8_t ch, uint8_t state)
{
	switch (ch) {
	case LED_GREEN:
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, state);
		break;
	case LED_LINK:
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, state);
		break;
	case LED_ERR:
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, state);
		break;
	default:
		break;
	}
}

int LedStop(uint8_t ch)
{
	if ((LedQueueHandle == NULL) || (ch >= LED_COUNT)) {
		return -1;
	}
	LedHandle_t buf = { 0 };
	buf.ch = ch;
	buf.state = LED_STOP;

	BaseType_t ret = xQueueSend(LedQueueHandle, &buf, 2);
	return ret == pdTRUE ? 0 : -1;
}

int LedStart(uint8_t ch, uint16_t period)
{
	if ((LedQueueHandle == NULL) || (ch >= LED_COUNT)) {
		return -1;
	}

	LedHandle_t buf = { 0 };
	buf.period = period;
	buf.ch = ch;
	buf.state = LED_START;

	BaseType_t ret = xQueueSend(LedQueueHandle, &buf, 2);
	return ret == pdTRUE ? 0 : -1;
}

void LedTimerCallback(TimerHandle_t xTimer)
{
	if (xTimer == LedTimerHandle[LED_GREEN]) {
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		return;
	}
	if (xTimer == LedTimerHandle[LED_LINK]) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		return;
	}
	if (xTimer == LedTimerHandle[LED_ERR]) {
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		return;
	}
}