//
// Created by LuChenxu on 2022/8/28.
//

#include "attitude_task.h"
#include "mpu6050_raw.h"
#include "Fusion.h"
#include "physicalConstants.h"
#include "FreeRTOS.h"
#include "task.h"

#define SAMPLE_PERIOD 0.005

#define CONVERT_ADC_TO_G(x) x*8.0f/32768
#define CONVERT_ADC_TO_DEGREE(x) x*0.06103515625f
#define CONVERT_DEGREE_TO_RAD(x) CONVERT_ADC_TO_DEGREE(x)*0.01745329222f

FusionEuler euler;

osThreadId_t sensorMpu6050TaskHandle = NULL;
const osThreadAttr_t sensor_mpu6050_task_attr = {
        .name="mpu6050Task",
        .priority=(osPriority_t) osPriorityRealtime,
        .stack_size=128 * 4
};

osThreadId_t attitudeCalculateTaskHandle = NULL;
const osThreadAttr_t attitude_calculate_task_attr = {
        .name="attitude",
        .priority=(osPriority_t) osPriorityRealtime1,
        .stack_size=128 * 4
};

void mpu6050UpdateTask();

void attitudeCalculateTask(void *parg);

void mpu6050TaskCreate(void) {
    sensorMpu6050TaskHandle = osThreadNew(mpu6050UpdateTask, NULL, &sensor_mpu6050_task_attr);
//    attitudeCalculateTask(NULL);
}

void attitudeTaskCreate(void) {
    attitudeCalculateTaskHandle = osThreadNew(attitudeCalculateTask, NULL, &attitude_calculate_task_attr);
}

void mpu6050UpdateTask() {
    mpu6050Init();
    while (1) {
        mpu6050GetRawDataAsync(&gyro, &acc);
        osDelay(2);
    }
}

void attitudeCalculateTask(void *parg) {
    FusionAhrs ahrs;
    FusionAhrsInitialise(&ahrs);
    static portTickType xLastWakeTime;
    while (true) {

        // replace this with actual gyroscope data in degrees/s
        const FusionVector gyroscope = {CONVERT_ADC_TO_DEGREE(gyro.gx),
                                        CONVERT_ADC_TO_DEGREE(gyro.gy),
                                        CONVERT_ADC_TO_DEGREE(gyro.gz)};
        // replace this with actual accelerometer data in g
        const FusionVector accelerometer = {CONVERT_ADC_TO_G(acc.ax),
                                            CONVERT_ADC_TO_G(acc.ay),
                                            CONVERT_ADC_TO_G(acc.az)};

        FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, SAMPLE_PERIOD);

        euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5));

//        printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n", euler.angle.roll, euler.angle.pitch, euler.angle.yaw);
    }
}
