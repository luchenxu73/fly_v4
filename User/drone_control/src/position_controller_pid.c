/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie Firmware
 *
 * Copyright (C) 2016 Bitcraze AB
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
 * position_estimator_pid.c: PID-based implementation of the position controller
 */

#include <math.h>
#include "num.h"

// #include "log.h"
// #include "param.h"
#include "pid.h"
#include "num.h"
#include "position_controller.h"
#include "physicalConstants.h"

struct pidAxis_s {
    PidObject pid;

    stab_mode_t previousMode;
    float setpoint;

    float output;
};

struct this_s {
    struct pidAxis_s pidVX;
    struct pidAxis_s pidVY;
    struct pidAxis_s pidVZ;

    struct pidAxis_s pidX;
    struct pidAxis_s pidY;
    struct pidAxis_s pidZ;

    uint16_t thrustBase; // approximate throttle needed when in perfect hover. More weight/older battery can use a higher value
    uint16_t thrustMin;  // Minimum thrust value to output
};

// Maximum roll/pitch angle permited
static float rLimit = 20;
static float pLimit = 20;
static float rpLimitOverhead = 1.10f;
// Velocity maximums
static float xVelMax = 1.0f;
static float yVelMax = 1.0f;
static float zVelMax = 1.0f;
static float velMaxOverhead = 1.10f;
static const float thrustScale = 1000.0f;

// Feedforward gains
static float kFFx = 0.0; // feedforward gain for x direction [deg / m/s]
static float kFFy = 0.0; // feedforward gain for y direction [deg / m/s]

#define DT (float)(1.0f/POSITION_RATE)
bool posFiltEnable = true;
bool velFiltEnable = true;
float posFiltCutoff = 20.0f;
float velFiltCutoff = 20.0f;
bool posZFiltEnable = true;
bool velZFiltEnable = true;
float posZFiltCutoff = 20.0f;
#ifdef IMPROVED_BARO_Z_HOLD
float velZFiltCutoff = 0.7f;
#else
float velZFiltCutoff = 20.0f;
#endif

#ifndef UNIT_TEST
static struct this_s this = {
        .pidVX = {
                .pid = {
                        .kp = 25.0f,
                        .ki = 1.0f,
                        .kd = 0.0f,
                },
                .pid.dt = DT,
        },

        .pidVY = {
                .pid = {
                        .kp = 25.0f,
                        .ki = 1.0f,
                        .kd = 0.0f,
                },
                .pid.dt = DT,
        },
#ifdef IMPROVED_BARO_Z_HOLD
        .pidVZ = {
          .pid = {
            .kp = 3.0f,
            .ki = 1.0f,
            .kd = 1.5f, //kd can be lowered for improved stability, but results in slower response time.
          },
          .pid.dt = DT,
        },
#else
        .pidVZ = {
                .pid = {
                        .kp = 25.0f,
                        .ki = 15.0f,
                        .kd = 0,
                },
                .pid.dt = DT,
        },
#endif
        .pidX = {
                .pid = {
                        .kp = 2.0f,
                        .ki = 0.0f,
                        .kd = 0.0f,
                },
                .pid.dt = DT,
        },

        .pidY = {
                .pid = {
                        .kp = 2.0f,
                        .ki = 0.0f,
                        .kd = 0.0f,
                },
                .pid.dt = DT,
        },

        .pidZ = {
                .pid = {
                        .kp = 2.0f,
                        .ki = 0.5f,
                        .kd = 0.0f,
                },
                .pid.dt = DT,
        },
#ifdef IMPROVED_BARO_Z_HOLD
        .thrustBase = 38000,
#else
        .thrustBase = 36000,
#endif
        .thrustMin  = 20000,
};
#endif

void positionControllerInit() {
    pidInit(&this.pidX.pid, this.pidX.setpoint, this.pidX.pid.kp, this.pidX.pid.ki, this.pidX.pid.kd,
            this.pidX.pid.dt, POSITION_RATE, posFiltCutoff, posFiltEnable);
    pidInit(&this.pidY.pid, this.pidY.setpoint, this.pidY.pid.kp, this.pidY.pid.ki, this.pidY.pid.kd,
            this.pidY.pid.dt, POSITION_RATE, posFiltCutoff, posFiltEnable);
    pidInit(&this.pidZ.pid, this.pidZ.setpoint, this.pidZ.pid.kp, this.pidZ.pid.ki, this.pidZ.pid.kd,
            this.pidZ.pid.dt, POSITION_RATE, posZFiltCutoff, posZFiltEnable);

    pidInit(&this.pidVX.pid, this.pidVX.setpoint, this.pidVX.pid.kp, this.pidVX.pid.ki, this.pidVX.pid.kd,
            this.pidVX.pid.dt, POSITION_RATE, velFiltCutoff, velFiltEnable);
    pidInit(&this.pidVY.pid, this.pidVY.setpoint, this.pidVY.pid.kp, this.pidVY.pid.ki, this.pidVY.pid.kd,
            this.pidVY.pid.dt, POSITION_RATE, velFiltCutoff, velFiltEnable);
    pidInit(&this.pidVZ.pid, this.pidVZ.setpoint, this.pidVZ.pid.kp, this.pidVZ.pid.ki, this.pidVZ.pid.kd,
            this.pidVZ.pid.dt, POSITION_RATE, velZFiltCutoff, velZFiltEnable);
}

static float runPid(float input, struct pidAxis_s *axis, float setpoint, float dt) {
    axis->setpoint = setpoint;

    pidSetDesired(&axis->pid, axis->setpoint);
    return pidUpdate(&axis->pid, input, true);
}


float state_body_x, state_body_y, state_body_vx, state_body_vy;

void positionController(float *thrust, attitude_t *attitude, setpoint_t *setpoint,
                        const state_t *state) {
    this.pidX.pid.outputLimit = xVelMax * velMaxOverhead;
    this.pidY.pid.outputLimit = yVelMax * velMaxOverhead;
    // The ROS landing detector will prematurely trip if
    // this value is below 0.5
    this.pidZ.pid.outputLimit = fmaxf(zVelMax, 0.5f) * velMaxOverhead;

    float cosyaw = cosf(state->attitude.yaw * (float) M_PI / 180.0f);
    float sinyaw = sinf(state->attitude.yaw * (float) M_PI / 180.0f);

    float setp_body_x = setpoint->position.x * cosyaw + setpoint->position.y * sinyaw;
    float setp_body_y = -setpoint->position.x * sinyaw + setpoint->position.y * cosyaw;

    state_body_x = state->position.x * cosyaw + state->position.y * sinyaw;
    state_body_y = -state->position.x * sinyaw + state->position.y * cosyaw;

    float globalvx = setpoint->velocity.x;
    float globalvy = setpoint->velocity.y;

    //X, Y
    if (setpoint->mode.x == modeAbs) {
        setpoint->velocity.x = runPid(state_body_x, &this.pidX, setp_body_x, DT);
    } else if (!setpoint->velocity_body) {
        setpoint->velocity.x = globalvx * cosyaw + globalvy * sinyaw;
    }
    if (setpoint->mode.y == modeAbs) {
        setpoint->velocity.y = runPid(state_body_y, &this.pidY, setp_body_y, DT);
    } else if (!setpoint->velocity_body) {
        setpoint->velocity.y = globalvy * cosyaw - globalvx * sinyaw;
    }
    if (setpoint->mode.z == modeAbs) {
        setpoint->velocity.z = runPid(state->position.z, &this.pidZ, setpoint->position.z, DT);
    }

    velocityController(thrust, attitude, setpoint, state);
}

void velocityController(float *thrust, attitude_t *attitude, setpoint_t *setpoint,
                        const state_t *state) {
    this.pidVX.pid.outputLimit = pLimit * rpLimitOverhead;
    this.pidVY.pid.outputLimit = rLimit * rpLimitOverhead;
    // Set the output limit to the maximum thrust range
    this.pidVZ.pid.outputLimit = (UINT16_MAX / 2 / thrustScale);
    //this.pidVZ.pid.outputLimit = (this.thrustBase - this.thrustMin) / thrustScale;

    float cosyaw = cosf(state->attitude.yaw * (float) M_PI / 180.0f);
    float sinyaw = sinf(state->attitude.yaw * (float) M_PI / 180.0f);
    state_body_vx = state->velocity.x * cosyaw + state->velocity.y * sinyaw;
    state_body_vy = -state->velocity.x * sinyaw + state->velocity.y * cosyaw;

    // Roll and Pitch
    attitude->pitch = -runPid(state_body_vx, &this.pidVX, setpoint->velocity.x, DT) - kFFx * setpoint->velocity.x;
    attitude->roll = -runPid(state_body_vy, &this.pidVY, setpoint->velocity.y, DT) - kFFy * setpoint->velocity.y;

    attitude->roll = constrain(attitude->roll, -rLimit, rLimit);
    attitude->pitch = constrain(attitude->pitch, -pLimit, pLimit);

    // Thrust
    float thrustRaw = runPid(state->velocity.z, &this.pidVZ, setpoint->velocity.z, DT);
    // Scale the thrust and add feed forward term
    *thrust = thrustRaw * thrustScale + this.thrustBase;
    // Check for minimum thrust
    if (*thrust < this.thrustMin) {
        *thrust = this.thrustMin;
    }
    // saturate
    *thrust = constrain(*thrust, 0, UINT16_MAX);
}

void positionControllerResetAllPID() {
    pidReset(&this.pidX.pid);
    pidReset(&this.pidY.pid);
    pidReset(&this.pidZ.pid);
    pidReset(&this.pidVX.pid);
    pidReset(&this.pidVY.pid);
    pidReset(&this.pidVZ.pid);
}

void positionControllerResetAllfilters() {
    filterReset(&this.pidX.pid, POSITION_RATE, posFiltCutoff, posFiltEnable);
    filterReset(&this.pidY.pid, POSITION_RATE, posFiltCutoff, posFiltEnable);
    filterReset(&this.pidZ.pid, POSITION_RATE, posZFiltCutoff, posZFiltEnable);
    filterReset(&this.pidVX.pid, POSITION_RATE, velFiltCutoff, velFiltEnable);
    filterReset(&this.pidVY.pid, POSITION_RATE, velFiltCutoff, velFiltEnable);
    filterReset(&this.pidVZ.pid, POSITION_RATE, velZFiltCutoff, velZFiltEnable);
}

