#ifndef __MYOLED_H
#define __MYOLED_H
#include "device.h"
#include "oledlib.h"

#define OLED_NUM 1
#define VA_BUF_SIZE 256

/* OLED进程 */
void OLED_Task(void);

#endif // !__MYOLED_H
