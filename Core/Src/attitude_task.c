//
// Created by LuChenxu on 2022/8/28.
//

#include "attitude_task.h"
#include "mpu6050_raw.h"
osThreadId_t sensorMpu6050TaskHandle = NULL;
const osThreadAttr_t  sensor_mpu6050_task_attr = {
        .name="mpu6050Task",
        .priority=(osPriority_t) osPriorityRealtime,
        .stack_size=128 * 4
};
void mpu6050UpdateTask();
void mpu6050TaskCreate(void) {
    sensorMpu6050TaskHandle = osThreadNew(mpu6050UpdateTask, NULL, &sensor_mpu6050_task_attr);
}
void mpu6050UpdateTask()
{
    mpu6050Init();
    while (1)
    {
        mpu6050GetRawDataAsync(&gyro,&acc);
        osDelay(2);
    }

}
