#include "AppMain.h"

void MainThread(void *arg);

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
	if (osKernelInitialize()) {
		Error_Handler();
	}
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
	for (uint8_t i = 0; i < LED_ERR; i++) {
		if (LedStart(i, 1000 * (i + 1))) {
			WarningMessage();
		}
	}
	if (LedStop(LED_ERR)) {
		WarningMessage();
	}

	while (1) {
		vTaskDelay(1000);
	}
}