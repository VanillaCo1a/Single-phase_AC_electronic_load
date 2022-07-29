#ifndef __MY_TASK_H
#define __MY_TASK_H
#include "device.h"

#define VA_BUF_SIZE 256
#define UART_BUF_SIZE 2048

/* 最小调试系统进程 */
void DEBUG_SYSTEM_Task();
/* PFC测量模块进程 */
void HLW8032_OLED_Task(int32_t i);

#endif // !__MY_TASK_H
