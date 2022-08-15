#include "uart.h"
#include "usart.h"

/***  串口类, 初始化实例的样例如下:
static UART_ModuleHandleTypeDef uart_muart[UART_NUM] = {
    {.cmni = {
         .protocol = USART,
         .ware = HARDWARE,
#if defined(STM32)
#if defined(STM32HAL)
         .bus = &huart1,
#elif defined(STM32FWLIB)
         .bus = USART1,
#endif
#endif
     },
     .usedma = 0}};
static DEVS_TypeDef myuarts = {.type = UART};
static DEV_TypeDef myuart[] = {{.parameter = NULL, .io = {0}, .cmni = {.num = 1, .confi = (DEVCMNI_TypeDef *)&uart_muart[0], .init = NULL}}};    ***/


static DEVS_TypeDef *uarts = NULL;
static char *va_buf = NULL;
static size_t va_size = 0;

/* 串口构造函数 */
void UART_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize uSize, char *buf, size_t bSize) {
    uarts = devs;
    va_buf = buf;
    va_size = bSize;
    DEV_Init(devs, dev, uSize);
#if defined(STM32)
#if defined(STM32HAL)
#elif defined(STM32FWLIB)
    USART1_Confi();
#endif
#endif
}

/* TODO: 串口析构函数 */
void UART_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize size) {}

bool UART_ScanArray(int8_t num, uint8_t arr[], size_t size, size_t *length, DEV_StatusTypeDef wait) {
    DEV_SetActStream(uarts, num);
    return (DEVCMNI_Read((uint8_t *)arr, size, length, 0xFF) == wait);
}
bool UART_ScanString(int8_t num, char *str, size_t size, DEV_StatusTypeDef wait) {
    DEV_StatusTypeDef rc;
    bool res = false;
    size_t length;
    DEV_SetActStream(uarts, num);
    if((rc = DEVCMNI_Read((uint8_t *)str, size - 1, &length, 0xFF)) == wait) {
        res = true;
    }
    if(rc == DEV_OK) {
        str[length] = '\0';
    }
    return res;
}
bool UART_PrintArray(int8_t num, const uint8_t arr[], size_t size, DEV_StatusTypeDef wait) {
    DEV_SetActStream(uarts, num);
    return (DEVCMNI_Write((uint8_t *)arr, size, 0xFF) == wait);
}
bool UART_PrintString(int8_t num, const char *str, DEV_StatusTypeDef wait) {
    DEV_SetActStream(uarts, num);
    return (DEVCMNI_Write((uint8_t *)str, strlen(str), 0xFF) == wait);
}
bool UART_Printf(int8_t num, char *str, DEV_StatusTypeDef wait, ...) {
    va_list args;
    va_start(args, str);
    vsnprintf(va_buf, va_size, (char *)str, args);
    va_end(args);
    DEV_SetActStream(uarts, num);
    return UART_PrintString(0, va_buf, wait);
}
