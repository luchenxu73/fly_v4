//
// Created by LuChenxu on 2022/8/28.
//

#ifndef FLY_V4_ATTITUDE_TASK_H
#define FLY_V4_ATTITUDE_TASK_H

#include "cmsis_os.h"
#include "Fusion.h"

extern FusionEuler euler;
extern osThreadId_t sensorMpu6050TaskHandle;

void mpu6050TaskCreate(void);

void attitudeTaskCreate(void);

#endif //FLY_V4_ATTITUDE_TASK_H
