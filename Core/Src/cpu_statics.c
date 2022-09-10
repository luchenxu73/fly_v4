//
// Created by LuChenxu on 2022/9/10.
//
#include "cpu_statics.h"
#include <stdio.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"
volatile uint32_t cpu_time = 0;
extern TIM_HandleTypeDef htim3;

TaskHandle_t cpuStaticInfoThread = NULL;
const osThreadAttr_t cpu_static_info_attr={
        .name="cpu info",
        .stack_size=128*4,
        .priority = (osPriority_t)osPriorityLow
};

void configureTimerForRunTimeStats(void)
{
    cpu_time = 0UL;
}

 unsigned long getRunTimeCounterValue(void)
{
    return cpu_time;
}

uint8_t CPU_RunInfo[512];
void cpuInfoTaskCreate(void)
{

    cpuStaticInfoThread=osThreadNew(cpuInfoTask,NULL,&cpu_static_info_attr);
}

/* cpuInfoTask function */
void cpuInfoTask(void *argument) {
    /* USER CODE BEGIN cpuInfoTask */
    HAL_TIM_Base_Start_IT(&htim3);
    /* Infinite loop */
    while (true) {
//        memset(CPU_RunInfo, 0, 512);
//        vTaskList((char *) &CPU_RunInfo); //获取任务运行时间信息
//        printf("---------------------------------------------\r\n");
//        printf("任务名       任务状态     优先级     剩余栈     任务序号\r\n");
//        printf("%s", CPU_RunInfo);
//        printf("---------------------------------------------\r\n");
//        memset(CPU_RunInfo, 0, 512);
//        vTaskGetRunTimeStats((char *) &CPU_RunInfo);
//        printf("任务名         运行计数     使用率\r\n");
//        printf("%s", CPU_RunInfo);
//        printf("---------------------------------------------\r\n\n");
        osDelay(500); /* 延时500个tick */

    }
    /* USER CODE END cpuInfoTask */
}
