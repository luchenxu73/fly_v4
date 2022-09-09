//
// Created by LuChenxu on 2022/8/28.
//

#ifndef FLY_V4_ATTITUDE_TASK_H
#define FLY_V4_ATTITUDE_TASK_H

#include "cmsis_os.h"
extern osThreadId_t sensorTaskHandle ;
void mpu6050TaskCreate(void) ;
#endif //FLY_V4_ATTITUDE_TASK_H
