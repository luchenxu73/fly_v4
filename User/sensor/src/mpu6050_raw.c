#include "mpu6050_raw.h"

#define SMOOTH_WINDOW_SIZE 8
// #define AXIS_MAX 6
int16_t smooth_buffer[3][SMOOTH_WINDOW_SIZE];
uint8_t cur_idx = 0;

GyroscopeData gyro;
AccelerometerData acc;

void mpu6050WriteByte(uint8_t add, uint8_t reg, uint8_t dat) {
    HAL_I2C_Mem_Write(&I2CHandle, add, reg, I2C_MEMADD_SIZE_8BIT, &dat, 1, HAL_MAX_DELAY);
}

uint8_t mpu6050ReadByte(uint8_t add, uint8_t reg) {
    uint8_t dat;
    HAL_I2C_Mem_Read(&I2CHandle, add, reg, I2C_MEMADD_SIZE_8BIT, &dat, 1, HAL_MAX_DELAY);
    return dat;
}

void mpu6050ReadBuffer(uint8_t add, uint8_t reg, uint8_t *dat, uint32_t len) {
    HAL_I2C_Mem_Read(&I2CHandle, add, reg, I2C_MEMADD_SIZE_8BIT, dat, len, HAL_MAX_DELAY);
}

void mpu6050Init() {
    uint8_t dummy_read_time;
    int16_t dummy_ret[6];

    // Power
    mpu6050WriteByte(MPU6050_DEV_ADDR, MPU6050_REG_PWR_MGMT_1, 0x00);
    while (mpu6050ReadByte(MPU6050_DEV_ADDR, MPU6050_REG_PWR_MGMT_1) & 0x80) {
        // wait until reset done.
        ;
    }

    // Low pass filter.
    mpu6050WriteByte(MPU6050_DEV_ADDR, MPU6050_REG_CONFIG, MPU6050_DLPF_CFG1);

    // enable DLPF so Freq = 1k , with div4 ,sample rate: 1k/(1+4) = 200Hz.
    mpu6050WriteByte(MPU6050_DEV_ADDR, MPU6050_REG_SMPLRT_DIV, MPU6050_SMPLRT_DIV4);

    // +- 2000 dps, disable self test.
    mpu6050WriteByte(MPU6050_DEV_ADDR, MPU6050_REG_GYRO_CONFIG, MPU6050_FS_SEL_PN2000);

    // +-8 g acc.
    mpu6050WriteByte(MPU6050_DEV_ADDR, MPU6050_REG_ACC_CONFIG, MPU6050_AFS_SEL_PN8);

    // through.
    mpu6050WriteByte(MPU6050_DEV_ADDR, MPU6050_REG_INT_PIN_CFG, 0x02);

    // disable iic master mode.
    mpu6050WriteByte(MPU6050_DEV_ADDR, MPU6050_REG_USER_CTRL, 0x00);

//    for (dummy_read_time = 0; dummy_read_time < SMOOTH_WINDOW_SIZE; dummy_read_time++)
//    {
//        mpu6050ReadFilteredData(&dummy_ret[0], &dummy_ret[1], &dummy_ret[2], &dummy_ret[3], &dummy_ret[4], &dummy_ret[6]);
//    }
}

void mpu6050GetRawData(GyroscopeData *gyro, AccelerometerData *acc) {
    uint8_t buffer[6];
    mpu6050ReadBuffer(MPU6050_DEV_ADDR, MPU6050_REG_GYRO_XOUT_H, buffer, 6);
    gyro->gx = (int16_t) ((int16_t) buffer[0] << 8 | buffer[1]);
    gyro->gy = (int16_t) ((int16_t) buffer[2] << 8 | buffer[3]);
    gyro->gz = (int16_t) ((int16_t) buffer[4] << 8 | buffer[5]);

    mpu6050ReadBuffer(MPU6050_DEV_ADDR, MPU6050_REG_ACCEL_XOUT_H, buffer, 6);
    acc->ax = (int16_t) ((int16_t) buffer[0] << 8 | buffer[1]);
    acc->ay = (int16_t) ((int16_t) buffer[2] << 8 | buffer[3]);
    acc->az = (int16_t) ((int16_t) buffer[4] << 8 | buffer[5]);
}

void mpu6050ReadFilteredData(GyroscopeData *gyro, AccelerometerData *acc) {
    uint8_t i, j;
    int16_t ret[3];
    mpu6050GetRawData(gyro, acc);
    smooth_buffer[0][cur_idx] = gyro->gx;
    smooth_buffer[1][cur_idx] = gyro->gy;
    smooth_buffer[2][cur_idx] = gyro->gz;

    cur_idx = (cur_idx == SMOOTH_WINDOW_SIZE - 1 ? 0 : cur_idx + 1);

    for (i = 0; i < 3; i++) {
        int32_t sum = 0;
        for (j = 0; j < SMOOTH_WINDOW_SIZE; j++) {
            sum += smooth_buffer[i][j];
        }
        ret[i] = sum / SMOOTH_WINDOW_SIZE;
    }

    gyro->gx = ret[0];
    gyro->gy = ret[1];
    gyro->gz = ret[2];
}