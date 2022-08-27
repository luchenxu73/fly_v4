//
// Created by LuChenxu on 2022/8/26.
//

#ifndef FLY_V4_BLINK_TASK_H
#define FLY_V4_BLINK_TASK_H
#include "cmsis_os.h"

extern osThreadId_t ledBlinkTaskHandle;
//void ledBlinkTask(void* parg);
void ledBlinkTaskCreate(void);

#endif //FLY_V4_BLINK_TASK_H
