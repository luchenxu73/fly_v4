//
// Created by LuChenxu on 2022/8/27.
//
#include "display_task.h"
#include "ssd1306.h"
#include "mpu6050_raw.h"
//#include "Fusion.h"
#include "attitude_task.h"
osThreadId_t displayTaskHandle = NULL;
const osThreadAttr_t display_attributes = {
        .name = "Display Task",
        .stack_size = 128 * 4,
        .priority = (osPriority_t) osPriorityBelowNormal,
};

void displayTask(void *parg);


void displayTaskCreate(void) {
    displayTaskHandle = osThreadNew(displayTask, NULL, &display_attributes);
}

void displayTask(void *parg) {
    ssd1306_Init();
    ssd1306_Fill(Black);
//    ssd1306_UpdateScreen();
//    ssd1306UpdateScreenAsync();
    int a = 0;
    while (1) {
//        ssd1306_SetCursor(0, a);

//        ssd1306Printf(0,0,"%d %u",gyro.gx,getRunTimeCounterValue());
        ssd1306Printf(0,8,"%d",(int )(euler.angle.pitch*100));
//        ssd1306Printf(64,8,"%.3f",euler.angle.roll);


//        ssd1306_SetCursor(0, a);
//        ssd1306_WriteString("hello", Font_6x8, White);
//        ssd1306_UpdateScreen();
//        ssd1306UpdateScreenAsync();
        a += 8;
        osDelay(pdMS_TO_TICKS(10));
    }
}