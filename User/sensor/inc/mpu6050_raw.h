#ifndef MPU6050_RAW_H
#define MPU6050_RAW_H

#include "stdint.h"
#include "stm32f1xx_hal.h"

#define I2CHandle hi2c1
extern I2C_HandleTypeDef I2CHandle;

// 定义MPU6050内部地址
#define MPU6050_DEV_ADDR 0x68 << 1 // IIC写入时的地址字节数据，+1为读取

// register map
#define MPU6050_REG_SMPLRT_DIV 0x19
#define MPU6050_REG_CONFIG 0x1A
#define MPU6050_REG_GYRO_CONFIG 0x1B
#define MPU6050_REG_ACC_CONFIG 0x1C
#define MPU6050_REG_INT_PIN_CFG 0x37
#define MPU6050_REG_TEMP_H 0x41
#define MPU6050_REG_TEMP_L 0x42
#define MPU6050_REG_SIGNAL_PATH_RESET 0x68
#define MPU6050_REG_USER_CTRL 0x6A
#define MPU6050_REG_PWR_MGMT_1 0x6B

#define MPU6050_REG_GYRO_XOUT_H 0x43
#define MPU6050_REG_ACCEL_XOUT_H 0x3B

// register mask

// sample rate =  FREQ / ( DIV + 1 )
#define MPU6050_SMPLRT_DIV1 (0x01)
#define MPU6050_SMPLRT_DIV2 (0x02)
#define MPU6050_SMPLRT_DIV3 (0x03)
#define MPU6050_SMPLRT_DIV4 (0x04)
#define MPU6050_SMPLRT_DIV5 (0x05)
#define MPU6050_SMPLRT_DIV6 (0x06)
#define MPU6050_SMPLRT_DIV7 (0x07)
#define MPU6050_SMPLRT_DIV8 (0x08)

#define MPU6050_DLPF_CFG0 (0x00)
#define MPU6050_DLPF_CFG1 (0x01)
#define MPU6050_DLPF_CFG2 (0x02)
#define MPU6050_DLPF_CFG3 (0x03)
#define MPU6050_DLPF_CFG4 (0x04)
#define MPU6050_DLPF_CFG5 (0x05)
#define MPU6050_DLPF_CFG6 (0x06)
#define MPU6050_DLPF_CFG7 (0x07)

#define MPU6050_FS_SEL_PN2000 (0x03 << 3)
#define MPU6050_FS_SEL_PN1000 (0x02 << 3)
#define MPU6050_FS_SEL_PN500 (0x01 << 3)
#define MPU6050_FS_SEL_PN250 (0x00 << 3)

#define MPU6050_XG_ST (0x01 << 7)
#define MPU6050_YG_ST (0x01 << 6)
#define MPU6050_ZG_ST (0x01 << 5)

#define MPU6050_XA_ST (0x01 << 7)
#define MPU6050_YA_ST (0x01 << 6)
#define MPU6050_ZA_ST (0x01 << 5)

#define MPU6050_AFS_SEL_PN2 (0x00 << 3)
#define MPU6050_AFS_SEL_PN4 (0x01 << 3)
#define MPU6050_AFS_SEL_PN8 (0x02 << 3)
#define MPU6050_AFS_SEL_PN16 (0x03 << 3)

#define MPU6050_RST (0x01 << 7)
#define MPU6050_SLEEP (0x01 << 6)
#define MPU6050_TMP_DIS (0x01 << 3)


typedef struct GyroscopeData {
    volatile int16_t gx;
    volatile int16_t gy;
    volatile int16_t gz;
} GyroscopeData;

typedef struct AccelerometerData {
    volatile int16_t ax;
    volatile int16_t ay;
    volatile int16_t az;
} AccelerometerData;

extern GyroscopeData gyro;
extern AccelerometerData acc;

void mpu6050WriteByte(uint8_t add, uint8_t reg, uint8_t dat);

uint8_t mpu6050ReadByte(uint8_t add, uint8_t reg);

void mpu6050ReadBuffer(uint8_t add, uint8_t reg, uint8_t *dat, uint32_t len);


void mpu6050Init(void);

void mpu6050GetRawData(GyroscopeData* gyro,AccelerometerData* acc);

void mpu6050ReadFilteredData(GyroscopeData* gyro,AccelerometerData* acc);

#endif