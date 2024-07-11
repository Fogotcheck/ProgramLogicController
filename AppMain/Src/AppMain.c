#include "AppMain.h"

void MainThread(void *arg);

void AppMain(void)
{
    BaseType_t ret =
        xTaskCreate(MainThread, "MainTask", 200, NULL, 1, NULL);
    if (ret != pdPASS)
    {
        Error_Handler();
    }
    vTaskStartScheduler();
}

void MainThread(__attribute__((unused)) void *arg)
{
    while (1)
    {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        vTaskDelay(100);
    }
}