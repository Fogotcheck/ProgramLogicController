#include "AppMain.h"

EventGroupHandle_t MainEvent = NULL;

void MainThread(void *arg);
void MainEventHandler(EventBits_t Event);

void AppMain(void)
{
	BaseType_t ret =
		xTaskCreate(MainThread, "MainTask", 500, NULL, 1, NULL);
	if (ret != pdPASS) {
		Error_Handler();
	}
	vTaskStartScheduler();
}

void MainThread(__attribute__((unused)) void *arg)
{
	/* init main */
	if (osKernelInitialize()) {
		Error_Handler();
	}

	MainEvent = xEventGroupCreate();
	if (MainEvent == NULL) {
		Error_Handler();
	}

	/* init modules*/
	if (DLogInit()) {
		Error_Handler();
	}

	if (LedInit()) {
		ErrMessage();
		Error_Handler();
	}
	if (MX_LWIP_Init()) {
		ErrMessage();
		Error_Handler();
	}

	InfoMessage("Init::OK");
	vTaskDelay(1000);
	/* start setiings */
	for (uint8_t i = 0; i < LED_ERR; i++) {
		if (LedStart(i, 1000 * (i + 1))) {
			WarningMessage();
		}
	}
	if (LedStop(LED_ERR)) {
		WarningMessage();
	}
	EventBits_t Event = 0;
	EventBits_t Mask = 1;
	while (1) {
		Event = xEventGroupWaitBits(MainEvent, MAIN_ALL_EVENTS, pdFALSE,
					    pdFALSE, portMAX_DELAY);
		Mask = 1;
		for (uint8_t i = 0; i < configUSE_16_BIT_TICKS; i++) {
			if (Event & Mask) {
				MainEventHandler(Event & Mask);
			}
			Mask <<= 1;
		}
	}
}

void MainEventHandler(EventBits_t Event)
{
	xEventGroupClearBits(MainEvent, Event);
	InfoMessage("event::0x%x", Event);
	switch (Event) {
	case ETH_LINK_UP:
		LedStart(LED_LINK, 200);
		break;
	case ETH_LINK_DOWN:
		LedStart(LED_LINK, 2000);
		break;
	default:
		break;
	}
}