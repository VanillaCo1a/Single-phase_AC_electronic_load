#include "device.h"

#if defined(STM32)
#if defined(STM32HAL)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    TIMER_Callback(htim);
}
#elif defined(STM32FWLIB)
void TIM4_IRQHandler(void) {
    TIMER_Callback(NULL);
}
#endif
#endif


/* 定时器进程 */
static bool firstRun = true;
void TIMER_Task(void) {
    if(firstRun) {
        TIMER_Init();
        firstRun = false;
    }
}
