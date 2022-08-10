#ifndef __MYDEVICE_H
#define __MYDEVICE_H
#include <stdlib.h>

/* 运行环境宏定义 */
#if !defined(STM32) && !defined(STC89C) && !defined(TC264) && \
    !defined(TI) && !defined(ESP32) && !defined(HC32)
#define STM32
// #define STC89C
// #define TC264
// #define TI
// #define ESP32
// #define HC32
#endif
#if defined(STM32)
#define STM32HAL
// #define STM32FWLIB
#define USE_LLLIB
#define USE_REGISTER
#elif defined(STC89C)
// #define STC89C51
#define STC89C52
#endif

/* 设备定时器宏定义 */
#if defined(STM32)
#if defined(STM32HAL)
#include "tim.h"
#define TIMERERROR 0    //为了修正函数运行带来的误差, 在使用不同型号芯片前需调试计算得到数值
#define TIMERCOUNT TIM6->CNT
#define TIMERHANDLE htim6
#elif defined(STM32FWLIB)
#define TIMERERROR 0
#define TIMER TIM4
#define TIMERCOUNT TIM4->CNT
#define TIMER_IRQn TIM4_IRQn
#define RCC_TIMER RCC_APB1Periph_TIM4
#endif

/* 通信使用宏定义 */
#define DEVI2C_SOFTWARE_ENABLED
#define DEVSPI_SOFTWARE_ENABLED
// #define DEVUART_SOFTWARE_ENABLED
// #define DEVOWRE_SOFTWARE_ENABLED
#define DEVI2C_HARDWARE_ENABLED
#define DEVSPI_HARDWARE_ENABLED
#define DEVUART_HARDWARE_ENABLED
// #define DEVOWRE_HARDWARE_ENABLED
// #define DEVI2C_USEPOINTER
// #define DEVSPI_USEPOINTER
// #define DEVUART_USEPOINTER
// #define DEVOWRE_USEPOINTER

/* 设备池尺寸宏定义 */
#define DEV_DEFINE
#define DEVPOOL_MAXNUM     100    //设备池大小
#define DEVBUSYLIST_MAXNUM 20     //忙设备列表大小
typedef uint16_t poolsize;        //池大小的数据类型
#endif

#endif // !__MYDEVICE_H
