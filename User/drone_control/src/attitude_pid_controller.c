/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie Firmware
 *
 * Copyright (C) 2011-2012 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * attitude_pid_controller.c: Attitude controller using PID correctors
 */
#include <stdbool.h>

#include "stabilizer_types.h"
#include "attitude_controller.h"
#include "pid.h"

#define ATTITUDE_LPF_CUTOFF_FREQ      15.0f
#define ATTITUDE_LPF_ENABLE false
#define ATTITUDE_RATE_LPF_CUTOFF_FREQ 30.0f
#define ATTITUDE_RATE_LPF_ENABLE false


static inline int16_t saturateSignedInt16(float in) {
    // don't use INT16_MIN, because later we may negate it, which won't work for that value.
    if (in > INT16_MAX)
        return INT16_MAX;
    else if (in < -INT16_MAX)
        return -INT16_MAX;
    else
        return (int16_t) in;
}

PidObject pidRollRate = {
        .kp = PID_ROLL_RATE_KP,
        .ki = PID_ROLL_RATE_KI,
        .kd = PID_ROLL_RATE_KD,
};

PidObject pidPitchRate = {
        .kp = PID_PITCH_RATE_KP,
        .ki = PID_PITCH_RATE_KI,
        .kd = PID_PITCH_RATE_KD,
};

PidObject pidYawRate = {
        .kp = PID_YAW_RATE_KP,
        .ki = PID_YAW_RATE_KI,
        .kd = PID_YAW_RATE_KD,
};

PidObject pidRoll = {
        .kp = PID_ROLL_KP,
        .ki = PID_ROLL_KI,
        .kd = PID_ROLL_KD,
};

PidObject pidPitch = {
        .kp = PID_PITCH_KP,
        .ki = PID_PITCH_KI,
        .kd = PID_PITCH_KD,
};

PidObject pidYaw = {
        .kp = PID_YAW_KP,
        .ki = PID_YAW_KI,
        .kd = PID_YAW_KD,
};

// fed to the motor
// should be accessible by other units
int16_t rollOutput;
int16_t pitchOutput;
int16_t yawOutput;

// fed to roll/pitch/yaw pid
// accessible when need to debug
static float rollRateDesired;
static float pitchRateDesired;
static float yawRateDesired;

static bool isInit = false;

void attitudeControllerInit(float updateDt) {
    if (isInit)
        return;

    //TODO: get parameters from configuration manager instead
    pidInit(&pidRollRate, 0, pidRollRate.kp, pidRollRate.ki, pidRollRate.kd,
            updateDt, ATTITUDE_RATE, ATTITUDE_RATE_LPF_CUTOFF_FREQ, ATTITUDE_RATE_LPF_ENABLE);
    pidInit(&pidPitchRate, 0, pidPitchRate.kp, pidPitchRate.ki, pidPitchRate.kd,
            updateDt, ATTITUDE_RATE, ATTITUDE_RATE_LPF_CUTOFF_FREQ, ATTITUDE_RATE_LPF_ENABLE);
    pidInit(&pidYawRate, 0, pidYawRate.kp, pidYawRate.ki, pidYawRate.kd,
            updateDt, ATTITUDE_RATE, ATTITUDE_RATE_LPF_CUTOFF_FREQ, ATTITUDE_RATE_LPF_ENABLE);

    pidSetIntegralLimit(&pidRollRate, PID_ROLL_RATE_INTEGRATION_LIMIT);
    pidSetIntegralLimit(&pidPitchRate, PID_PITCH_RATE_INTEGRATION_LIMIT);
    pidSetIntegralLimit(&pidYawRate, PID_YAW_RATE_INTEGRATION_LIMIT);

    pidInit(&pidRoll, 0, pidRoll.kp, pidRoll.ki, pidRoll.kd, updateDt,
            ATTITUDE_RATE, ATTITUDE_LPF_CUTOFF_FREQ, ATTITUDE_LPF_ENABLE);
    pidInit(&pidPitch, 0, pidPitch.kp, pidPitch.ki, pidPitch.kd, updateDt,
            ATTITUDE_RATE, ATTITUDE_LPF_CUTOFF_FREQ, ATTITUDE_LPF_ENABLE);
    pidInit(&pidYaw, 0, pidYaw.kp, pidYaw.ki, pidYaw.kd, updateDt,
            ATTITUDE_RATE, ATTITUDE_LPF_CUTOFF_FREQ, ATTITUDE_LPF_ENABLE);

    pidSetIntegralLimit(&pidRoll, PID_ROLL_INTEGRATION_LIMIT);
    pidSetIntegralLimit(&pidPitch, PID_PITCH_INTEGRATION_LIMIT);
    pidSetIntegralLimit(&pidYaw, PID_YAW_INTEGRATION_LIMIT);

    isInit = true;
}

bool attitudeControllerTest() {
    return isInit;
}

void attitudeControllerCorrectRatePID(
        float rollRateActual, float pitchRateActual, float yawRateActual) {
    pidSetDesired(&pidRollRate, rollRateDesired);
    rollOutput = saturateSignedInt16(pidUpdate(&pidRollRate, rollRateActual, true));

    pidSetDesired(&pidPitchRate, pitchRateDesired);
    pitchOutput = saturateSignedInt16(pidUpdate(&pidPitchRate, pitchRateActual, true));

    pidSetDesired(&pidYawRate, yawRateDesired);
    yawOutput = saturateSignedInt16(pidUpdate(&pidYawRate, yawRateActual, true));
}

void attitudeControllerCorrectAttitudePID(
        float eulerRollActual, float eulerPitchActual, float eulerYawActual,
        float eulerRollDesired, float eulerPitchDesired, float eulerYawDesired) {
    pidSetDesired(&pidRoll, eulerRollDesired);
    rollRateDesired = pidUpdate(&pidRoll, eulerRollActual, true);

    // Update PID for pitch axis
    pidSetDesired(&pidPitch, eulerPitchDesired);
    pitchRateDesired = pidUpdate(&pidPitch, eulerPitchActual, true);

    // Update PID for yaw axis
    float yawError;
    yawError = eulerYawDesired - eulerYawActual;
    if (yawError > 180.0f)
        yawError -= 360.0f;
    else if (yawError < -180.0f)
        yawError += 360.0f;
    pidSetError(&pidYaw, yawError);
    yawRateDesired = pidUpdate(&pidYaw, eulerYawActual, false);
}

void attitudeControllerResetRollAttitudePID(void) {
    pidReset(&pidRoll);
}

void attitudeControllerResetPitchAttitudePID(void) {
    pidReset(&pidPitch);
}

void attitudeControllerResetAllPID(void) {
    pidReset(&pidRoll);
    pidReset(&pidPitch);
    pidReset(&pidYaw);
    pidReset(&pidRollRate);
    pidReset(&pidPitchRate);
    pidReset(&pidYawRate);
}

void attitudeControllerGetActuatorOutput(int16_t *roll, int16_t *pitch, int16_t *yaw) {
    *roll = rollOutput;
    *pitch = pitchOutput;
    *yaw = yawOutput;
}
