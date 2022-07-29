#include "myuart.h"
#include "uart.h"

/* 设备参数配置 */
static UART_ModuleHandleTypeDef msuart[UART_NUM] = {
    {.receive = {.state = 0}, .transmit = {.state = 1}, .usedma = 1},
    {.receive = {.state = 0}, .transmit = {.state = 1}, .usedma = 0}};
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
static DEVS_TypeDef myuarts = {.type = UART};
static DEV_TypeDef myuart[UART_NUM] = {
    {.parameter = NULL, .io = {0}, .cmni = {.num = 1, .confi = (DEVCMNI_TypeDef *)&uart_cmni[0], .init = NULL}},
    {.parameter = NULL, .io = {0}, .cmni = {.num = 1, .confi = (DEVCMNI_TypeDef *)&uart_cmni[1], .init = NULL}}};

/* 初始化sprintf缓冲区 */
static char my_va_buf[VA_BUF_SIZE];

/* 初始化串口缓冲区 */
static uint8_t uart_buf[UART_BUF_SIZE];

/* 方法重写 */
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
void UART1_Printf(char *str, ...) {
    va_list args;
    va_start(args, str);
    UART_Printf(0, str, args);
    va_end(args);
}
bool UART3_ScanArray(uint8_t arr[], size_t size, size_t *length) {
    return UART_ScanArray(1, arr, size, length);
}
bool HLW8032_Read(uint8_t arr[], size_t size, size_t *length) {
    return UART3_ScanArray(arr, size, length);
}

/* printf重定向 */
int fputc(int ch, FILE *f) {
    /* 这里需要加一个volatile, 不然会被AC6编译优化掉 */
    volatile bool res = 0;
    DEVS_TypeDef *devs = DEV_getActDevs();
    poolsize dev = DEV_getActStream();
    DEV_setActStream(&myuarts, 0);
    while(!(res = DEVCMNI_Write((uint8_t *)&ch, 1, 0xFF))) continue;
    DEV_setActStream(devs, dev);
    return ch;
}
int _write(int fd, char *pBuffer, int size) {
    volatile bool res = 0;
    DEVS_TypeDef *devs = DEV_getActDevs();
    poolsize dev = DEV_getActStream();
    DEV_setActStream(&myuarts, 0);
    while(!(res = DEVCMNI_Write((uint8_t *)pBuffer, size, 0xFF))) continue;
    DEV_setActStream(devs, dev);
    return size;
}


/* 串口进程 */
static bool firstRun = true;
void UART_Task(void) {
    if(firstRun) {
        UART_Init(&myuarts, myuart, sizeof(myuart) / sizeof(*myuart), my_va_buf, sizeof(my_va_buf));
        printf("UART1初始化完毕\r\n");
        delayms_timer(5);
        printf("UART3初始化完毕\r\n");
        firstRun = false;
    }
    
    DEV_setActStream(&myuarts, 0);
    while(DEV_getActState() == idle) {
        /* 串口回显 */
        if(UART1_ScanString((char *)uart_buf, sizeof(uart_buf))) {
            UART1_PrintString((char *)uart_buf);
        }

        /* 置忙500ms */
        DEV_setActState(25000);
    }
}
