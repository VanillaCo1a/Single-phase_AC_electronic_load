#include "uart.h"
#include "usart.h"

static UART_ModuleHandleTypeDef msuart[UART_NUM] = {
    {.receive = {.state = 0}, .transmit = {.state = 1}, .usedma = 0},
};
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
};
DEVS_TypeDef uarts = {.type = UART};
DEV_TypeDef uart[UART_NUM] = {
    {.parameter = NULL, .io = {0}, .cmni = {.num = 1, .confi = (DEVCMNI_TypeDef *)&uart_cmni[0], .init = NULL}}};

/* 串口初始化函数 */
void UART_Init(void) {
    DEV_Init(&uarts, uart, sizeof(uart) / sizeof(*uart));
#if defined(STM32)
#if defined(STM32HAL)
#elif defined(STM32FWLIB)
    USART1_Confi();
#endif
#endif
    printf("UART1初始化完毕\r\n");
}


/* TOFIX: 由于设备序号和设备号的对应关系处理还不是很完善, 这个函数还不能作为对外的调用接口 */
static inline bool UART_ScanArray(int8_t num, uint8_t arr[], size_t size, size_t *length) {
    DEV_setActStream(&uarts, num);
    return DEVCMNI_Read((uint8_t *)arr, size, length, 0xFF);
}
static inline bool UART_ScanString(int8_t num, char *str, size_t size) {
    bool res = false;
    size_t length;
    DEV_setActStream(&uarts, num);
    res = DEVCMNI_Read((uint8_t *)str, size - 1, &length, 0xFF);
    if(res) {
        str[length] = '\0';
    }
    return res;
}
static inline void UART_PrintArray(int8_t num, const uint8_t arr[], size_t size) {
    DEV_setActStream(&uarts, num);
    while(!DEVCMNI_Write((uint8_t *)arr, size, 0xFF)) continue;
}
static inline void UART_PrintString(int8_t num, const char *str) {
    DEV_setActStream(&uarts, num);
    while(!DEVCMNI_Write((uint8_t *)str, strlen(str), 0xFF)) continue;
}

char va_buf[VA_BUF_SIZE] = {0};
bool UART1_ScanArray(uint8_t arr[], size_t size, size_t *length) {
    return UART_ScanArray(0, arr, size, length);
}
bool UART1_ScanString(char *str, size_t size) {
    return UART_ScanString(0, str, size);
}
void UART1_PrintArray(const uint8_t arr[], size_t size) {
    UART_PrintArray(0, arr, size);
}
void UART1_PrintString(const char *str) {
    UART_PrintString(0, str);
}
void UART1_Printf(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(va_buf, sizeof(va_buf), (char *)fmt, args);
    va_end(args);

    UART_PrintString(0, va_buf);
}

/* printf重定向 */
int fputc(int ch, FILE *f) {
    /* 这里需要加一个volatile, 不然会被AC6编译优化掉 */
    volatile bool res = 0;
    DEVS_TypeDef *devs = DEV_getActDevs();
    poolsize dev = DEV_getActStream();
    DEV_setActStream(&uarts, 0);
    while(!(res = DEVCMNI_Write((uint8_t *)&ch, 1, 0xFF))) continue;
    DEV_setActStream(devs, dev);
    return ch;
}
int _write(int fd, char *pBuffer, int size) {
    volatile bool res = 0;
    DEVS_TypeDef *devs = DEV_getActDevs();
    poolsize dev = DEV_getActStream();
    DEV_setActStream(&uarts, 0);
    while(!(res = DEVCMNI_Write((uint8_t *)pBuffer, size, 0xFF))) continue;
    DEV_setActStream(devs, dev);
    return size;
}
