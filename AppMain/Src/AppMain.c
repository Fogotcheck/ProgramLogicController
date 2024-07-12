#include "AppMain.h"

void MainThread(void *arg);

void AppMain(void)
{
	BaseType_t ret =
		xTaskCreate(MainThread, "MainTask", 200, NULL, 1, NULL);
	if (ret != pdPASS) {
		Error_Handler();
	}
	vTaskStartScheduler();
}

void MainThread(__attribute__((unused)) void *arg)
{
	if (LedInit()) {
		Error_Handler();
	}
	vTaskDelay(1000);
	for (uint8_t i = 0; i < LED_ERR; i++) {
		LedStart(i, 1000 * (i + 1));
	}
	LedStop(LED_ERR);

	while (1) {
		vTaskDelay(100);
	}
}