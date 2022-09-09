//
// Created by LuChenxu on 2022/8/28.
//

#include "attitude_task.h"
#include "mpu6050_raw.h"
osThreadId_t sensorTaskHandle = NULL;
const osThreadAttr_t  sensor_task_attr = {
        .name="mpu6050Task",
        .priority=(osPriority_t) osPriorityRealtime,
        .stack_size=128 * 4
};
void mpu6050UpdateTask();
void mpu6050TaskCreate(void) {
    sensorTaskHandle = osThreadNew(mpu6050UpdateTask,NULL,&sensor_task_attr);
}
void mpu6050UpdateTask()
{
//    mpu6050Init();
    while (1)
    {
        osDelay(2);
    }

}
