#include "device_timer.h"

volatile uint64_t _time = 0;
void TIMER_tick(void) {
    _time = TIMER_getRunTimeus();
}
uint64_t TIMER_query(void) {
    return TIMER_getRunTimeus() - _time;
}
float TIMER_fps(void) {
    return 1000000.0/(TIMER_getRunTimeus() - _time);
}


//定时器配置部分, 默认使用TIM4, 向上计数模式, 周期1ms
//2^64-1us约为58w年, 基本够用了, 因此不考虑数据溢出归零涉及的延时计算问题
volatile uint64_t time_us = 0, time_ms = 0;
volatile int8_t flag_timerrupt = 0;
uint32_t TIMER_getRunTimems(void) {
    return time_ms;
}
uint32_t TIMER_getRunTimes(void) {
    return time_ms / 1000;
}


/***    第一次调用函数时开始计时, 后续调用时返回比较结果, 延时未结束返回0, 结束则返回1
注意: 每个延时函数需要定义单独的静态参数供函数使用, 内存开销较大,
考虑过减小*compare的大小, 但是经过考虑决定节省在取余和除法上的3us开销, 尽可能地提高精度

函数不适合直接调用, 应通过封装好的宏函数进行调用
调用方法例(详见头文件):
    { static uint32_t compare = 0;   static int8_t state = 0;
    TIMER_uscmptor(TIME, &compare, &state); }        ***/
//state0.输入:空
//获取当前时间, 记录当前时间, 更新state
//state1.输入:时间 返回:0/1(并复位state)
//获取当前时间, 比较历史时间
int8_t TIMER_mscmptor(uint64_t ms, volatile uint64_t *compare, volatile int8_t *state) {
    extern volatile uint64_t time_us;
    extern volatile int8_t flag_timerrupt;
    uint64_t time = 0;
    int8_t result = 0;
    if(*state == 0) {
        flag_timerrupt = 1;
        *compare = time_us + TIMERCOUNT;
        if(flag_timerrupt == 0) {
            *compare = time_us + TIMERCOUNT;
        }
        (*state)++;
    }
    flag_timerrupt = 1;
    time = time_us + TIMERCOUNT;
    if(flag_timerrupt == 0) {
        time = time_us + TIMERCOUNT;
    }
    if(time + TIMERERROR >= *compare + ms * 1000) {
        (*state)--;
        result = 1;
    }
    return result;
}
int8_t TIMER_scmptor(uint64_t s, volatile uint64_t *compare, volatile int8_t *state) {
    extern volatile uint64_t time_us;
    extern volatile int8_t flag_timerrupt;
    uint64_t time = 0;
    int8_t result = 0;
    if(*state == 0) {
        flag_timerrupt = 1;
        *compare = time_us + TIMERCOUNT;
        if(flag_timerrupt == 0) {
            *compare = time_us + TIMERCOUNT;
        }
        (*state)++;
    }
    flag_timerrupt = 1;
    time = time_us + TIMERCOUNT;
    if(flag_timerrupt == 0) {
        time = time_us + TIMERCOUNT;
    }
    if(time + TIMERERROR >= *compare + s * 1000000) {
        (*state)--;
        result = 1;
    }
    return result;
}


#if defined(STM32)
#if defined(STM32HAL)
TIM_HandleTypeDef *htimer = &TIMERHANDLE;
void TIMER_Init(void) {
    HAL_TIM_Base_Start_IT(htimer);
}
void TIMER_Callback(void *handle) {
    if(handle == htimer) {
        time_ms++;
        time_us += 1000;
        flag_timerrupt = 0;
    }
}
#elif defined(STM32FWLIB)
void TIMER_Init_(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    RCC_ClocksTypeDef RCC_ClocksStructure;
    RCC_GetClocksFreq(&RCC_ClocksStructure);
    RCC_APB1PeriphClockCmd(RCC_TIMER, ENABLE);
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = RCC_ClocksStructure.PCLK2_Frequency / 1000000 - 1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIMER, &TIM_TimeBaseInitStructure);
}
void TIMER_NVIC_Init(void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIMER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void TIMER_Init(void) {
#ifndef NVICGROUP
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#endif
    TIMER_Init_();
    TIMER_NVIC_Init();
    TIM_ITConfig(TIMER, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIMER, ENABLE);
}
void TIMER_Callback(void *handle) {
    if(TIM_GetITStatus(TIMER, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIMER, TIM_IT_Update);
        time_ms++;
        time_us += 1000;
        flag_timerrupt = 0;
    }
}
#endif
#endif
