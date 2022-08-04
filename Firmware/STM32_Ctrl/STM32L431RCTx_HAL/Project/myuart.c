#include "myuart.h"

/* 设备参数配置 */
static UART_ModuleHandleTypeDef msuart[UART_NUM] = {
    {.usedma = 0},
    {.usedma = 0}};
static DEVCMNI_TypeDef uart_cmni[UART_NUM] = {
    {.protocol = USART, .ware = HARDWARE, .modular = &msuart[0],
#if defined(STM32)
#if defined(STM32HAL)
     .bus = &huart1
#elif defined(STM32FWLIB)
     .bus = USART1
#endif
#endif
    },
    {.protocol = USART, .ware = HARDWARE, .modular = &msuart[1],
#if defined(STM32)
#if defined(STM32HAL)
     .bus = &huart3
#elif defined(STM32FWLIB)
     .bus = USART3
#endif
#endif
    }};
DEVS_TypeDef myuarts = {.type = UART};
DEV_TypeDef myuart[UART_NUM] = {
    {.parameter = NULL, .io = {0}, .cmni = {.num = 1, .confi = (DEVCMNI_TypeDef *)&uart_cmni[0], .init = NULL}},
    {.parameter = NULL, .io = {0}, .cmni = {.num = 1, .confi = (DEVCMNI_TypeDef *)&uart_cmni[1], .init = NULL}}};

/* 方法重写 */
bool UART1_ScanArray(uint8_t arr[], size_t size, size_t *length) {
    return UART_ScanArray(0, arr, size, length);
}
bool UART1_ScanString(char *str, size_t size) {
    return UART_ScanString(0, str, size);
}
bool UART1_PrintArray(const uint8_t arr[], size_t size) {
    return UART_PrintArray(0, arr, size);
}
bool UART1_PrintString(const char *str) {
    return UART_PrintString(0, str);
}
bool UART1_Printf(char *str, ...) {
    bool res;
    va_list args;
    va_start(args, str);
    res = UART_Printf(0, str, args);
    va_end(args);
    return res;
}
bool UART3_ScanArray(uint8_t arr[], size_t size, size_t *length) {
    return UART_ScanArray(1, arr, size, length);
}

/* printf&scanf重定向 */
int fputc(int ch, FILE *f) {
    DEVS_TypeDef *devs = DEV_getActDevs();
    poolsize dev = DEV_getActStream();
    DEV_setActStream(&myuarts, 0);
    while(!DEVCMNI_Write((uint8_t *)&ch, 1, 0xFF)) continue;
    DEV_setActStream(devs, dev);
    return ch;
}
int _write(int fd, char *pBuffer, int size) {
    DEVS_TypeDef *devs = DEV_getActDevs();
    poolsize dev = DEV_getActStream();
    DEV_setActStream(&myuarts, 0);
    while(!DEVCMNI_Write((uint8_t *)pBuffer, size, 0xFF)) continue;
    DEV_setActStream(devs, dev);
    return size;
}
int fgetc(FILE*f) {
    int ch;
    size_t length;
    DEVS_TypeDef *devs = DEV_getActDevs();
    poolsize dev = DEV_getActStream();
    DEV_setActStream(&myuarts, 0);
    while(!DEVCMNI_Read((uint8_t *)&ch, 1, &length, 0xFF)) continue;
    DEV_setActStream(devs, dev);
	return ch;
}
