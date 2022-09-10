//
// Created by LuChenxu on 2022/9/10.
//

#ifndef FLY_V4_CPU_STATICS_H
#define FLY_V4_CPU_STATICS_H
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

extern TaskHandle_t cpuStaticInfoThread;
extern volatile uint32_t cpu_time;
void cpuInfoTaskCreate(void);
void cpuInfoTask(void * argument);

//void configureTimerForRunTimeStats(void);
//unsigned long getRunTimeCounterValue(void);
#endif //FLY_V4_CPU_STATICS_H
