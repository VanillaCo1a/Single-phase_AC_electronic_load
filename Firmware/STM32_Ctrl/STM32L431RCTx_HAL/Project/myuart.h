#ifndef __MYUART_H
#define __MYUART_H
#include "device.h"

#define UART_NUM 2
#define VA_BUF_SIZE 256
#define UART_BUF_SIZE 2048

bool UART1_ScanArray(uint8_t *arr, size_t size, size_t *length);
bool UART1_ScanString(char *str, size_t size);
void UART1_PrintArray(const uint8_t *arr, size_t size);
void UART1_PrintString(const char *str);
void UART1_Printf(char *str, ...);
bool UART3_ScanArray(uint8_t arr[], size_t size, size_t *length);
bool HLW8032_Read(uint8_t arr[], size_t size, size_t *length);

/* 串口进程 */
void UART_Task(void);

#endif // !__MYUART_H
