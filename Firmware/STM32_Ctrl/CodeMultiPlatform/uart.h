#ifndef __UART_H
#define __UART_H
#include "device.h"

void UART_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize uSize, char *buf, size_t bSize);
/* TODO: 串口析构函数 */
void UART_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize size);
/* 串口通信函数 */
bool UART_ScanArray(int8_t num, uint8_t arr[], size_t size, size_t *length);
bool UART_ScanString(int8_t num, char *str, size_t size);
bool UART_PrintArray(int8_t num, const uint8_t arr[], size_t size);
bool UART_PrintString(int8_t num, const char *str);
bool UART_Printf(int8_t num, char *str, ...);

#endif
