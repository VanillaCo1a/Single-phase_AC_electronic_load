#ifndef __MYUART_H
#define __MYUART_H
#include "device.h"
#include "uart.h"

#define UART_NUM 2

extern DEVS_TypeDef myuarts;
extern DEV_TypeDef myuart[UART_NUM];

bool UART1_ScanArray(uint8_t *arr, size_t size, size_t *length);
bool UART1_ScanString(char *str, size_t size);
void UART1_PrintArray(const uint8_t *arr, size_t size);
void UART1_PrintString(const char *str);
void UART1_Printf(char *str, ...);
bool UART3_ScanArray(uint8_t arr[], size_t size, size_t *length);

#endif // !__MYUART_H
