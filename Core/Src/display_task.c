//
// Created by LuChenxu on 2022/8/27.
//
#include "display_task.h"
#include "ssd1306.h"


osThreadId_t displayTaskHandle;
const osThreadAttr_t display_attributes = {
        .name = "Blink Task",
        .stack_size = 128 * 4,
        .priority = (osPriority_t) osPriorityAboveNormal,
};

void displayTask(void *parg);


void displayTaskCreate(void) {
    osThreadNew(displayTask, NULL, &display_attributes);
}

void displayTask(void *parg) {
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    int a = 0;
    while (1) {
        ssd1306_SetCursor(0,a);
        ssd1306_WriteString("hello",Font_6x8,White);
        ssd1306_UpdateScreen();
        a+=7;
        osDelay(pdMS_TO_TICKS(100));
    }
}