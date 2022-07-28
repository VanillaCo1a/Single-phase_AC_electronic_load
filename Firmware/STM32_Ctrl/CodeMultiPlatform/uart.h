#ifndef __UART_H
#define __UART_H
#include "device.h"

#define UART_NUM 2
void UART_Init(void);

bool UART1_ScanArray(uint8_t *arr, size_t size, size_t *length);
bool UART1_ScanString(char *str, size_t size);
void UART1_PrintArray(const uint8_t *arr, size_t size);
void UART1_PrintString(const char *str);
void UART1_Printf(char *str, ...);
bool UART3_ScanArray(uint8_t arr[], size_t size, size_t *length);

#endif
