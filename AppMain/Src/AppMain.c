#include "AppMain.h"

void AppMain(void)
{
    while (1)
    {
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
        HAL_Delay(100);
    }
}