//
// Created by LuChenxu on 2022/8/26.
//
#include "user_tasks.h"
#include "FreeRTOS.h"
#include "main.h"
#include "task.h"
#include "cmsis_os.h"

void ledBlinkTask(void *parg) {
    while (1) {

        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
        osDelay(1000);
    }

}