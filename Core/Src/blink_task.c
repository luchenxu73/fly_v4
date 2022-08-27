//
// Created by LuChenxu on 2022/8/26.
//
#include "blink_task.h"
#include "main.h"
#include "cmsis_os.h"

void ledBlinkTask(void *parg);

osThreadId_t ledBlinkTaskHandle;
const osThreadAttr_t leb_blink_attributes = {
        .name = "Blink Task",
        .stack_size = 128 * 4,
        .priority = (osPriority_t) osPriorityNormal,
};

void ledBlinkTaskCreate(void)
{
    ledBlinkTaskHandle = osThreadNew(ledBlinkTask, NULL, &leb_blink_attributes);
}

void ledBlinkTask(void *parg) {
    while (1) {

        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
        osDelay(1000);
    }

}