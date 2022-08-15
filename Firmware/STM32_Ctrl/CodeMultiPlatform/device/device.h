#ifndef __DEVICE_H
#define __DEVICE_H

/*****   C基础头文件   *****/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>

/*****   配置头文件&宏定义   *****/
#include "mydevice.h"
#ifndef __MYDEVICE_H

/* 运行环境宏定义 */
#if !defined(STM32) && !defined(STC89C) && !defined(TC264) && \
    !defined(TI) && !defined(ESP32) && !defined(HC32)
// #define STM32
// #define STC89C
// #define TC264
// #define TI
// #define ESP32
// #define HC32
#endif
#if defined(STM32)
// #define STM32HAL
// #define STM32FWLIB
// #define USE_LLLIB
// #define USE_REGISTER
#elif defined(STC89C)
// #define STC89C51
// #define STC89C52
#endif

/* 设备定时器宏定义 */
#if defined(STM32)
#if defined(STM32HAL)
#include "tim.h"
#define TIMERERROR  0    //为了修正函数运行带来的误差, 在使用不同型号芯片前需调试计算得到数值
#define TIMERCOUNT  TIM1->CNT
#define TIMERHANDLE htim1
#elif defined(STM32FWLIB)
#define TIMERERROR 0
#define TIMER      TIM1
#define TIMERCOUNT TIM1->CNT
#define TIMER_IRQn TIM1_IRQn
#define RCC_TIMER  RCC_APB1Periph_TIM1
#endif
#endif

/* 通信使用宏定义 */
// #define DEVI2C_SOFTWARE_ENABLED
// #define DEVSPI_SOFTWARE_ENABLED
// #define DEVUART_SOFTWARE_ENABLED
// #define DEVOWRE_SOFTWARE_ENABLED
// #define DEVI2C_HARDWARE_ENABLED
// #define DEVSPI_HARDWARE_ENABLED
// #define DEVUART_HARDWARE_ENABLED
// #define DEVOWRE_HARDWARE_ENABLED
// #define DEVI2C_USEPOINTER
// #define DEVSPI_USEPOINTER
// #define DEVUART_USEPOINTER
// #define DEVOWRE_USEPOINTER

/* 设备池宏定义 */
#define DEVPOOL_MAXNUM     0    //设备池大小
#define DEVBUSYLIST_MAXNUM 0    //忙设备列表大小
typedef uint16_t poolsize;      //池大小的数据类型

#endif    // !__MYDEVICE_H


/*****   枚举   *****/
typedef enum {
    DEV_POOLFULL = 1,
    DEV_NOFOUND,
    DEVIO_NOFOUND,
    DEVCMNI_NOFOUND,
    DEVCMNIIO_NOFOUND,
} DEV_ErrorTypeDef;
typedef enum {
    DEVUNDEF = 0,
    CMNIBUS,
    OLED,
    MPU6050,
    DS18B20,
    UART,
    OTHER,
} DEV_TypeTypeDef;
typedef enum {
    I2C = 1,
    SPI,
    USART,
    ONEWIRE,
} DEVCMNI_ProtocolTypeDef;
typedef enum {
    idle = 0,
    busy = !idle,
} DEV_StateTypeDef;
typedef enum {
#if defined(STM32)
#if defined(STM32HAL)
    DEVIO_PIN_RESET = GPIO_PIN_RESET,
    DEVIO_PIN_SET = GPIO_PIN_SET
#else
    DEVIO_PIN_RESET = 0,
    DEVIO_PIN_SET = !DEVIO_PIN_RESET
#endif
#endif
} DEVIO_PinState;
typedef enum {
    SOFTWARE = 1,
    HARDWARE,
} DEVCMNI_WareTypeDef;


/*****   结构体定义   *****/
/* 设备IO结构体定义 */
typedef struct {
    DEVIO_PinState State;
#if defined(STM32)
    GPIO_InitTypeDef Structure;
#endif
} DEVIO_InitTypeDef;
typedef struct {
#if defined(STM32)
#if defined(STM32FWLIB)
    uint32_t CLK;
#endif
    GPIO_TypeDef *GPIOx;
#if defined(USE_LLLIB) && defined(USE_FULL_LL_DRIVER)
    uint32_t GPIO_Pin;
#else
    uint16_t GPIO_Pin;
#endif
    DEVIO_InitTypeDef Init;
#endif
} DEVIO_TypeDef;

/* 设备通信结构体定义 */
typedef struct {    //设备通信IO结构体
    DEVIO_TypeDef SCL_SCK;
    DEVIO_TypeDef SDA_SDO_TXD_OWRE;
    DEVIO_TypeDef SDI_RXD;
    DEVIO_TypeDef CS;
} DEVCMNIIO_TypeDef;
typedef struct {                         //设备通信结构体
    DEVCMNI_ProtocolTypeDef protocol;    //总线协议类型
    DEVCMNI_WareTypeDef ware;            //总线驱动方式
    void *bus;                           //总线句柄
} DEVCMNI_TypeDef;

/* 设备总体控制 */
typedef struct {             //设备类结构体
    DEV_TypeTypeDef type;    //设备类型
    poolsize stream;         //活动设备相对序号
    poolsize pool;           //活动设备绝对序号
    poolsize size;           //当前设备数量
} DEVS_TypeDef;
typedef struct {
    uint8_t num;
    DEVIO_TypeDef *confi;
    void (*init)(void);
} DEV_IOTypeDef;
typedef struct {
    uint8_t num;
    // uint8_t numnow;
    DEVCMNI_TypeDef *confi;
    void (*init)(void);
} DEV_CMNITypeDef;
typedef struct {             //设备结构体
    DEV_TypeTypeDef type;    //设备类型
    poolsize state;          //设备忙闲状态
    int8_t error;            //设备错误标志
    DEV_IOTypeDef io;        //设备IO配置
    DEV_CMNITypeDef cmni;    //设备通信配置
    void *parameter;
} DEV_TypeDef;


/*****   设备子文件的头文件   *****/
#include "device_timer.h"
#include "device_protocol.h"


/*****   设备相关函数   *****/
/* 设备构造&错误处理部分 */
void DEV_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize size);
void DEV_Error(uint16_t err);
/* 活动流控制部分 */
int8_t DEV_SetActDevs(DEVS_TypeDef *self);
DEVS_TypeDef *DEV_GetActDevs(void);
int8_t DEV_SetActStream(DEVS_TypeDef *self, poolsize stream);
poolsize DEV_GetActStream(void);
DEV_TypeDef *DEV_GetActDev(void);
DEVIO_TypeDef *DEV_GetActDevIo(void);
DEVCMNI_TypeDef *DEV_GetActDevCmni(void);
DEVCMNIIO_TypeDef *DEV_GetActDevCmniIo(void);
void DEV_CloseActStream(void);
int8_t DEV_SetStream(DEVS_TypeDef *self, poolsize stream);
DEV_TypeDef *DEV_GetStream(DEVS_TypeDef *self);
int8_t DEV_SetActState(uint16_t us);
DEV_StateTypeDef DEV_GetActState(void);
DEVCMNI_TypeDef *DEV_GetCmni(void *bus);
void DEV_DoAction(DEVS_TypeDef *devs, void (*action)(void));
/* 设备IO部分 */
void DEVIO_SetPin(DEVIO_TypeDef *devio);
void DEVIO_ResetPin(DEVIO_TypeDef *devio);
void DEVIO_WritePin(DEVIO_TypeDef *devio, DEVIO_PinState pinstate);
DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *devio);
/* 设备通信部分 */
void DEVCMNI_WriteByte(uint8_t data, uint8_t address);
uint8_t DEVCMNI_ReadByte(uint8_t address);
bool DEVCMNI_ReadBit(uint8_t address);
DEV_StatusTypeDef DEVCMNI_Write(uint8_t *pdata, size_t size, uint8_t address);
DEV_StatusTypeDef DEVCMNI_Read(uint8_t *pdata, size_t size, size_t *length, uint8_t address);


/*****   运行环境头文件   *****/
#if defined(STM32)
#if defined(STM32HAL)
#include "main.h"
#elif defined(STM32FWLIB)
// #if defined(STM32F1)
// #include "stm32f10x.h"
// #elif defined(STM32F4)
// #include "stm32f4xx.h"
// #endif
#include "stm32f10x.h"
#endif
#endif

#endif    // !__DEVICE_H
