//
// Created by LuChenxu on 2022/8/27.
//

#ifndef FLY_V4_DISPLAY_TASK_H
#define FLY_V4_DISPLAY_TASK_H
#include "cmsis_os.h"
extern osThreadId_t displayTaskHandle;
void displayTaskCreate(void);

#endif //FLY_V4_DISPLAY_TASK_H
