#include "mytask.h"
#include "myuart.h"
#include "myoled.h"
#include "hlw8032.h"
#include "inverter.h"

#define lenof(arr) (sizeof(arr) / sizeof(*arr))

/* 初次运行标志位 */
static bool firstRun = true;
/* 运行帧率计算 */
float fps = 0;

/* 初始化sprintf缓冲区 */
static char my_va_buf[VA_BUF_SIZE];
/* 初始化串口缓冲区 */
static uint8_t my_uart_buf[UART_BUF_SIZE];

/* 最小调试系统进程 */
bool uartReceive = false;
void DEBUG_SYSTEM_Task(void) {
    if(firstRun) {
        TIMER_Init();
        UART_Init(&myuarts, myuart, lenof(myuart), my_va_buf, lenof(my_va_buf));
        printf("定时器初始化完毕\r\n");
        printf("UART1初始化完毕\r\n");
        printf("UART3初始化完毕\r\n");
        OLED_Init(&myoleds, myoled, lenof(myoled), my_va_buf, lenof(my_va_buf));
        printf("OLED初始化完毕\r\n");
        firstRun = false;
    }

    /* 串口回显 */
    DEV_setActStream(&myuarts, 0);
    if(DEV_getActState() == idle) {
        // 阻塞式，不常使用
        // fgets((char *)my_uart_buf, lenof(my_uart_buf), stdin);
        // fputs((char *)my_uart_buf, stdin);
        if(!uartReceive) {
            if(UART1_ScanString((char *)my_uart_buf, lenof(my_uart_buf))) {
                UART1_ScanString((char *)my_uart_buf, lenof(my_uart_buf));
                uartReceive = true;
            }
        }
        if(uartReceive) {
            if(UART1_PrintString((char *)my_uart_buf)) {
                UART1_PrintString((char *)my_uart_buf);
                uartReceive = false;
                /* 置忙500ms */
                DEV_setActState(25000);
            }
        }
    }

    DEV_setActStream(&myoleds, 0);
    if(DEV_getActState() == idle) {
        /* oled显示 */
        TIMER_tick();
        // OLED_Error();
        OLED_updateScreen();
        fps = TIMER_query();
        /* 置忙10ms */
        DEV_setActState(500);
    }
}

void HLW8032_OLED_Task(int32_t i) {
    extern uint16_t inverterNum;
    int8_t oled_mode = 0;
    HLW8032_ResTypedef *pfcres = HLW8032_GetResult();

    /* 显示循环次数 */
    switch(oled_mode) {
    case 0:
        OLED_clearBuffer();
        OLED_Printf(0 + 0 * 8, 0 + 0 * 8, "L431RBT6%8d", i);
        break;
    case 1:
        OLED_clearBuffer();
        OLED_Printf(0 + 0 * 8, 0 + 0 * 8, "L431RBT6%8d", i);
        break;
    case 2:
        break;

    default:
        break;
    }

    /* 获取8032数据 */
    if(HLW8032_Ctrl()) {

        /* 串口输出 */
        if(delayms_Timer_paral(500)) {
            printf("V=%7.3lfV  I=%7.3lfA  P=%7.3lfW  PFC=%6.4lf  W=%7.3lfkWh\r\n",
                   pfcres->voltage / DEC_BIT, pfcres->currentIntensity / DEC_BIT,
                   pfcres->power / DEC_BIT, pfcres->powerFactorer / DEC_BIT,
                   pfcres->electricQuantity / DEC_BIT);
        }

        /*  更新显存 */
        if(delayus_Timer_paral(100)) {
            /* 显示PFC测量数据 */
            switch(oled_mode) {
            case 0:
                OLED_Printf(0 + 0 * 8, 0 + 0 * 8, "L431RBT6%8d", inverterNum);
                SetFontSize(0);
                OLED_Printf(0 + 2 * 8, 0 + 0 * 8, "V=%4.1lfV  I=%4.1lfA",
                            pfcres->voltage / DEC_BIT, pfcres->currentIntensity / DEC_BIT);
                OLED_Printf(0 + 4 * 8, 0 + 0 * 8, "P=%4.1lfW  PFC=%3.1lf",
                            pfcres->power / DEC_BIT, pfcres->powerFactorer / DEC_BIT);
                OLED_Printf(0 + 6 * 8, 0 + 0 * 8, "W=%4.1lfkWh",
                            pfcres->electricQuantity / DEC_BIT);
                break;
            case 1:
                OLED_Printf(0 + 0 * 8, 0 + 0 * 8, "L431RBT6%8d", inverterNum);
                SetFontSize(1);
                OLED_Printf(0 + 3 * 8, 0 + 0 * 8, "V=%7.3lfV I=%7.3lfA",
                            pfcres->voltage / DEC_BIT, pfcres->currentIntensity / DEC_BIT);
                OLED_Printf(0 + 4 * 8, 0 + 0 * 8, "P=%7.3lfW PFC=%6.4lf",
                            pfcres->power / DEC_BIT, pfcres->powerFactorer / DEC_BIT);
                OLED_Printf(0 + 5 * 8, 0 + 0 * 8, "W=%7.3lfkWh",
                            pfcres->electricQuantity / DEC_BIT);
                SetFontSize(0);
                break;
            case 2:
                OLED_clearBuffer();
                SetFontSize(2);
                OLED_Printf(0 + 0 * 8, 0 + 0 * 8, "V=%7.3lfV", pfcres->voltage / DEC_BIT);
                OLED_Printf(0 + 2 * 8, 0 + 0 * 8, "I=%7.3lfA", pfcres->currentIntensity / DEC_BIT);
                OLED_Printf(0 + 4 * 8, 0 + 0 * 8, "P=%7.3lfW", pfcres->power / DEC_BIT);
                OLED_Printf(0 + 6 * 8, 0 + 0 * 8, "PFC=%6.4lf", pfcres->powerFactorer / DEC_BIT);
                SetFontSize(0);
                break;

            default:
                break;
            }
        }
    }
}

/* 定时器中断回调 */
#if defined(STM32)
#if defined(STM32HAL)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    TIMER_Callback(htim);
    INVERTER_Callback(htim);
}
#elif defined(STM32FWLIB)
void TIM4_IRQHandler(void) {
    TIMER_Callback(NULL);
}
#endif
#endif
