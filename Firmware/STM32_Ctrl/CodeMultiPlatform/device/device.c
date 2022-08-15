#include "device.h"

/*****   局部函数   *****/
static bool isDevNull(DEV_TypeDef *dev);
static bool isDevsNull(DEVS_TypeDef *devs, DEV_TypeDef dev[]);
static inline bool isDevIoNull(DEVIO_TypeDef *devio);
static bool isDevCmniNull(DEVCMNI_TypeDef *devcmni);
static bool isDevCmniBusNull(DEVCMNI_TypeDef *devcmni);
static bool isDevCmniIoNull(DEVCMNI_TypeDef *devcmni, DEVCMNIIO_TypeDef *devcmniio);
static bool initDevPool(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize size);

/*****   设备流控制部分   *****/
static poolsize _devSize = 0;
static DEV_TypeDef *_devPool[DEVPOOL_MAXNUM] = {0};
static uint64_t _devBusyList[DEVBUSYLIST_MAXNUM] = {0};    //忙设备表, 0为空, 非0为设备置忙时刻(us)

static DEVS_TypeDef *_actDevs = NULL;
static DEV_TypeDef *_actDev = NULL;
static DEVIO_TypeDef *_actDevIo = NULL;
static DEVCMNI_TypeDef *_actDevCmni = NULL;

/**
 * @description: 活动设备类设置: 设置活动设备类, 设备流根据活动设备类切换
 * @param {DEVS_TypeDef} *self
 * @return {*}
 */
int8_t DEV_SetActDevs(DEVS_TypeDef *self) {
    if(self == NULL) {
        return 1;
    }
    _actDevs = self;
    _actDev = _devPool[self->pool + self->stream];
    _actDevIo = _actDev->io.confi;
    _actDevCmni = _actDev->cmni.confi;
    return 0;
}
/**
 * @description: 活动设备类获取: 获取当前活动设备类
 * @param {*}
 * @return {*}_actDevs
 */
DEVS_TypeDef *DEV_GetActDevs(void) {
    return _actDevs;
}
/**
 * @description: 活动设备流设置: 设置活动设备类和设备流
 * @param {DEVS_TypeDef} *self
 * @param {poolsize} stream
 * @return {*}
 */
int8_t DEV_SetActStream(DEVS_TypeDef *self, poolsize stream) {
    if(self == NULL || stream > _devSize || stream > self->size) {
        return 1;
    }
    self->stream = stream;
    _actDevs = self;
    _actDev = _devPool[self->pool + self->stream];
    _actDevIo = _devPool[self->pool + self->stream]->io.confi;
    _actDevCmni = _devPool[self->pool + self->stream]->cmni.confi;
    return 0;
}
/**
 * @description: 活动设备流获取: 获取当前活动设备类的设备流
 * @param {*}
 * @return {*}_actDevs->stream
 */
inline poolsize DEV_GetActStream(void) {
    return _actDevs->stream;
}
/**
 * @description: 活动设备相关句柄获取
 * @param {*}
 * @return {*}
 */
inline DEV_TypeDef *DEV_GetActDev(void) {
    return _actDev;
}
inline DEVIO_TypeDef *DEV_GetActDevIo(void) {
    return _actDevIo;
}
inline DEVCMNI_TypeDef *DEV_GetActDevCmni(void) {
    return _actDevCmni;
}
inline DEVCMNIIO_TypeDef *DEV_GetActDevCmniIo(void) {
    return (DEVCMNIIO_TypeDef *)_actDevIo;
}
/**
 * @description: 关闭活动设备流
 * @param {*}
 * @return {*}
 */
void DEV_CloseActStream(void) {
    _actDev = NULL;
    _actDevs = NULL;
    _actDevIo = NULL;
    _actDevCmni = NULL;
}
/**
 * @description: 设备流设置: 设置某设备类的设备流
 * @param {DEVS_TypeDef} *self
 * @param {poolsize} stream
 * @return {*}
 */
int8_t DEV_SetStream(DEVS_TypeDef *self, poolsize stream) {
    if(self == NULL || stream > _devSize || stream > self->size) {
        return 1;
    }
    self->stream = stream;
    return 0;
}
/**
 * @description: 设备流获取: 获取某设备类的设备流
 * @param {DEVS_TypeDef} *self
 * @return {*}
 */
DEV_TypeDef *DEV_GetStream(DEVS_TypeDef *self) {
    if(self == NULL) {
        return NULL;
    }
    return _devPool[self->pool + self->stream];
}
/**
 * @description: 设置活动设备类设备的状态, us>0设置设备忙时长(单位为20us),us=0设置设备闲
 * @param {DEVS_TypeDef} *self
 * @param {uint16_t} twus
 * @return {*}
 */
int8_t DEV_SetActState(uint16_t twus) {
    if(twus > 0 && _actDev->state == 0) {                     //设置设备状态为忙
        for(poolsize i = 0; i < DEVBUSYLIST_MAXNUM; i++) {    //遍历设备表寻找空位
            if(_devBusyList[i] == 0) {
                _actDev->state = i + 1;    //设置成功, 保存当前设备忙时间记录在忙设备列表的第几个
                _devBusyList[i] = TIMER_getRunTimeus() + twus * 20;
                break;
            }
        }
        if(_actDev->state == 0) {
            return 1;    //设备状态表已满, 设置设备忙失败
        }
    } else if(twus > 0 && _actDev->state > 0) {    //更新设备置忙时间
        _devBusyList[_actDev->state - 1] = TIMER_getRunTimeus() + twus * 20;
    } else if(twus == 0 && _actDev->state > 0) {    //设置设备状态为闲
        _devBusyList[_actDev->state - 1] = 0;
        _actDev->state = 0;
    }
    return 0;
}
/**
 * @description: 获取设备类设备的状态, 先查询忙设备是否满足空闲条件, 然后更新状态并返回
 * @param {DEVS_TypeDef} *self
 * @return {*}
 */
DEV_StateTypeDef DEV_GetActState(void) {
    if(_actDev->state > 0 && TIMER_getRunTimeus() > _devBusyList[_actDev->state - 1]) {
        _devBusyList[_actDev->state - 1] = 0;    //设备忙指定时间后转为空闲状态
        _actDev->state = 0;
    }
    return _actDev->state == 0 ? idle : busy;
}
/**
 * @description: 根据某一设备的总线句柄查找该设备的通信句柄, 暂用于中断回调
 * @param {void} *bus
 * @return {*}
 */
DEVCMNI_TypeDef *DEV_GetCmni(void *bus) {
    for(size_t i = 0; i < _devSize; i++) {
        if(_devPool[i]->cmni.confi->bus == bus) {
            return _devPool[i]->cmni.confi;
        }
    }
    return NULL;
}
/**
 * @description: 对某一设备类批量进行某一操作
 * @param {DEVS_TypeDef} *devs
 * @param {void(*)()} *action
 * @return {*}
 */
void DEV_DoAction(DEVS_TypeDef *devs, void (*action)(void)) {
    for(poolsize i = 0; i < devs->size; i++) {
        DEV_SetActStream(devs, i);
        action();
    }
    DEV_CloseActStream();
}


/*****   设备IO部分   *****/
void DEVIO_Init(DEVIO_TypeDef *devio) {
    /* devio Clock Init */
#if defined(STM32)
#if defined(STM32HAL)
    if(devio->GPIOx == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if(devio->GPIOx == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if(devio->GPIOx == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
#if defined(GPIOD)
    else if(devio->GPIOx == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
#endif /* GPIOD */
#if defined(GPIOE)
    else if(devio->GPIOx == GPIOE) {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
#endif /* GPIOE */
#if defined(GPIOF)
    else if(devio->GPIOx == GPIOF) {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
#endif /* GPIOF */
#if defined(GPIOG)
    else if(devio->GPIOx == GPIOG) {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
#endif /* GPIOG */
#if defined(GPIOH)
    else if(devio->GPIOx == GPIOH) {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
#endif /* GPIOH */
#if defined(GPIOI)
    else if(devio->GPIOx == GPIOI) {
        __HAL_RCC_GPIOI_CLK_ENABLE();
    }
#endif /* GPIOI */
#if defined(GPIOJ)
    else if(devio->GPIOx == GPIOJ) {
        __HAL_RCC_GPIOJ_CLK_ENABLE();
    }
#endif /* GPIOJ */
#if defined(GPIOK)
    else if(devio->GPIOx == GPIOK) {
        __HAL_RCC_GPIOK_CLK_ENABLE();
    }
#endif /* GPIOK */
#elif defined(STM32FWLIB)
    RCC_APB2PeriphClockCmd(devio->CLK, ENABLE);
#endif
    /* GPIO Pin Set */
    DEVIO_WritePin(devio, devio->Init.State);
    /* GPIO Mode Configure */
#if defined(STM32HAL)
    devio->Init.Structure.Pin = devio->GPIO_Pin;
    HAL_GPIO_Init(devio->GPIOx, &devio->Init.Structure);
#else
    devio->Init.Structure.GPIO_Pin = devio->GPIO_Pin;
    GPIO_Init(devio->GPIOx, &devio->Init.Structure);
#endif
#endif
}

inline void DEVIO_SetPin(DEVIO_TypeDef *devio) {
#if defined(STM32)
#if defined(STM32HAL)
#if defined(USE_LLLIB) && defined(USE_FULL_LL_DRIVER)
    LL_GPIO_SetOutputPin(devio->GPIOx, devio->GPIO_Pin);
#elif defined(USE_REGISTER)
    devio->GPIOx->BSRR = devio->GPIO_Pin;
#else
    HAL_GPIO_WritePin(devio->GPIOx, devio->GPIO_Pin, GPIO_PIN_SET);
#endif
#elif defined(STM32FWLIB)
#if defined(USE_REGISTER)
    devio->GPIOx->BSRR = devio->GPIO_Pin;
#else
    GPIO_SetBits(devio->GPIOx, devio->GPIO_Pin);
#endif
#endif
#endif
}
inline void DEVIO_ResetPin(DEVIO_TypeDef *devio) {
#if defined(STM32)
#if defined(STM32HAL)
#if defined(USE_LLLIB) && defined(USE_FULL_LL_DRIVER)
    LL_GPIO_ResetOutputPin(devio->GPIOx, devio->GPIO_Pin);
#elif defined(USE_REGISTER)
    devio->GPIOx->BSRR = (uint32_t)(devio->GPIO_Pin << 16U);
#else
    HAL_GPIO_WritePin(devio->GPIOx, devio->GPIO_Pin, GPIO_PIN_RESET);
#endif
#elif defined(STM32FWLIB)
#if defined(USE_REGISTER)
    devio->GPIOx->BSRR = (uint32_t)devio->GPIO_Pin << 16U;
#else
    GPIO_ResetBits(devio->GPIOx, devio->GPIO_Pin);
#endif
#endif
#endif
}
inline void DEVIO_WritePin(DEVIO_TypeDef *devio, DEVIO_PinState pinstate) {
#if defined(STM32)
#if defined(STM32HAL)
#if defined(USE_LLLIB) && defined(USE_FULL_LL_DRIVER)
    if(pinstate != DEVIO_PIN_RESET)
        LL_GPIO_SetOutputPin(devio->GPIOx, devio->GPIO_Pin);
    else
        LL_GPIO_ResetOutputPin(devio->GPIOx, devio->GPIO_Pin);
#elif defined(USE_REGISTER)
    devio->GPIOx->BSRR = (uint32_t)(devio->GPIO_Pin << (!pinstate << 4));
    //#define BitBand(Addr, Bit) *((volatile int *)(((int)(Addr)&0x60000000) + 0x02000000 + (int)(Addr)*0x20 + (Bit)*4))
    //BitBand(&devio->GPIOx->ODR, pinstate == OLED0_SCK_Pin ? 13 : 15) = pinstate;
#else
    HAL_GPIO_WritePin(devio->GPIOx, devio->GPIO_Pin, (GPIO_PinState)pinstate);
#endif
#elif defined(STM32FWLIB)
#if defined(USE_REGISTER)
    devio->GPIOx->BSRR = (uint32_t)devio->GPIO_Pin << (!pinstate << 4);
#else
    if(pinstate != DEVIO_PIN_RESET)
        GPIO_SetBits(devio->GPIOx, devio->GPIO_Pin);
    else
        GPIO_ResetBits(devio->GPIOx, devio->GPIO_Pin);
#endif
#endif
#endif
}
inline DEVIO_PinState DEVIO_ReadPin(DEVIO_TypeDef *devio) {
#if defined(STM32)
#if defined(STM32HAL)
#if defined(USE_LLLIB) && defined(USE_FULL_LL_DRIVER)
    return (DEVIO_PinState)LL_GPIO_IsInputPinSet(devio->GPIOx, devio->GPIO_Pin);
#elif defined(USE_REGISTER)
    return (DEVIO_PinState)((devio->GPIOx->IDR & devio->GPIO_Pin) == devio->GPIO_Pin);
#else
    return (DEVIO_PinState)HAL_GPIO_ReadPin(devio->GPIOx, devio->GPIO_Pin);
#endif
#elif defined(STM32FWLIB)
#if defined(USE_REGISTER)
    return (DEVIO_PinState)((devio->GPIOx->IDR & devio->GPIO_Pin) == devio->GPIO_Pin);
#else
    return (DEVIO_PinState)GPIO_ReadInputDataBit(devio->GPIOx, devio->GPIO_Pin);
#endif
#endif
#endif
}


/*****   设备通信部分   *****/
//    模拟通信的方法绑定
__inline void DEVCMNI_SCL_Set(bool dir) {}
__inline void DEVCMNI_SDA_OWRE_Set(bool dir) {}
__inline void DEVCMNI_SCL_SCK_Out(bool pot) {
    DEVIO_WritePin(&DEV_GetActDevCmniIo()->SCL_SCK, (DEVIO_PinState)pot);
}
__inline void DEVCMNI_SDA_SDI_RXD_OWRE_Out(bool pot) {
    DEVIO_WritePin(&DEV_GetActDevCmniIo()->SDA_SDO_TXD_OWRE, (DEVIO_PinState)pot);
}
__inline bool DEVCMNI_SCL_In(void) {
    return DEVIO_ReadPin(&DEV_GetActDevCmniIo()->SCL_SCK);
}
__inline bool DEVCMNI_SDA_OWRE_In(void) {
    return DEVIO_ReadPin(&DEV_GetActDevCmniIo()->SDA_SDO_TXD_OWRE);
}
__inline bool DEVCMNI_SDO_In(void) {
    return DEVIO_ReadPin(&DEV_GetActDevCmniIo()->SDI_RXD);
}
__inline void DEVCMNI_CS_Out(bool pot) {
    DEVIO_WritePin(&DEV_GetActDevCmniIo()->CS, (DEVIO_PinState)pot);
}
void DEVCMNI_Error(int8_t err) {
    DEV_Error(_actDevs->type << 8 | err);
}
void DEVCMNI_Delayus(uint64_t us) {
    if(us) {
#if defined(STM32)
#if defined(STM32HAL)
        delayus_timer(us);
#elif defined(STM32FWLIB)
        delayus_timer(us);
#endif
#endif
    }
}
void DEVCMNI_Delayms(uint64_t ms) {
    if(ms) {
#if defined(STM32)
#if defined(STM32HAL)
#ifdef __TIMER_H
        delayms_timer(ms);
#else
        HAL_Delay(ms);
#endif
#elif defined(STM32FWLIB)
        delayms_timer(ms);
#endif
#endif
    }
}
int8_t DEVCMNI_Delayus_paral(uint64_t us) {
    if(us) {
#if defined(STM32)
#if defined(STM32HAL)
        return delayus_timer_paral(us);
#elif defined(STM32FWLIB)
        return delayus_timer_paral(us);
#endif
#endif
    }
    return 1;
}

//    I2C/SPI/ONEWIRE通信总线初始化函数
void DEVCMNI_BusInit(DEVCMNI_TypeDef *devcmni) {
    if(devcmni->protocol == I2C) {
        I2C_ModuleHandleTypeDef *devmdlr = (I2C_ModuleHandleTypeDef *)devcmni;
        if(devcmni->ware == SOFTWARE) {
#ifdef DEVI2C_USEPOINTER
            I2C_SoftHandleTypeDef *devbus = devmdlr->bus;
            devbus->SCL_Set = DEVCMNI_SCL_Set;
            devbus->SDA_Set = DEVCMNI_SDA_OWRE_Set;
            devbus->SCL_Out = DEVCMNI_SCL_SCK_Out;
            devbus->SDA_Out = DEVCMNI_SDA_SDI_RXD_OWRE_Out;
            devbus->SCL_In = DEVCMNI_SCL_In;
            devbus->SDA_In = DEVCMNI_SDA_OWRE_In;
            devbus->error = DEVCMNI_Error;
            devbus->delayus = DEVCMNI_Delayus;
            devbus->delayms = DEVCMNI_Delayms;
            devbus->delayus_paral = DEVCMNI_Delayus_paral;
#endif    // !DEVI2C_USEPOINTER
        }
    } else if(devcmni->protocol == SPI) {
        SPI_ModuleHandleTypeDef *devmdlr = (SPI_ModuleHandleTypeDef *)devcmni;
        if(devcmni->ware == SOFTWARE) {
#ifdef DEVSPI_USEPOINTER
            SPI_SoftHandleTypeDef *devbus = devmdlr->bus;
            devbus->SCK_Out = DEVCMNI_SCL_SCK_Out;
            devbus->SDI_RXD_Out = DEVCMNI_SDA_SDI_RXD_OWRE_Out;
            devbus->CS_Out = DEVCMNI_CS_Out;
            devbus->error = DEVCMNI_Error;
            devbus->delayus = DEVCMNI_Delayus;
            devbus->delayms = DEVCMNI_Delayms;
            devbus->delayus_paral = DEVCMNI_Delayus_paral;
#endif    // !DEVSPI_USEPOINTER
        }
    } else if(devcmni->protocol == USART) {
#ifdef DEVUART_USEPOINTER
        UART_ModuleHandleTypeDef *devmdlr = (UART_ModuleHandleTypeDef *)devcmni;
        if(devcmni->ware == SOFTWARE) {
        }
#endif    // !DEVUART_USEPOINTER
    } else if(devcmni->protocol == ONEWIRE) {
        ONEWIRE_ModuleHandleTypeDef *devmdlr = (ONEWIRE_ModuleHandleTypeDef *)devcmni;
        if(devcmni->ware == SOFTWARE) {
#ifdef DEVOWRE_USEPOINTER
            ONEWIRE_SoftHandleTypeDef *devbus = devmdlr->bus;
            devbus->OWIO_Set = DEVCMNI_SDA_OWRE_Set;
            devbus->OWIO_Out = DEVCMNI_SDA_SDI_RXD_OWRE_Out;
            devbus->OWIO_In = DEVCMNI_SDA_OWRE_In;
            devbus->error = DEVCMNI_Error;
            devbus->delayus = DEVCMNI_Delayus;
            devbus->delayms = DEVCMNI_Delayms;
            devbus->delayus_paral = DEVCMNI_Delayus_paral;
#endif    // !DEVOWRE_USEPOINTER
        }
    }
}
//    I2C/SPI/ONEWIRE通信设备初始化函数
void DEVCMNI_Init(DEVCMNI_TypeDef *devcmni, DEVCMNIIO_TypeDef *devcmniio) {
    //不同设备实际使用的通信引脚不同, 未定义的引脚不会被初始化
#if defined(STM32HAL)
    //HAL库的初始化可由CubeMX在main函数中完成, 此处会再次对通信的引脚进行初始化
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if(devcmni->protocol == I2C) {
        I2C_ModuleHandleTypeDef *devmdlr = (I2C_ModuleHandleTypeDef *)devcmni;
        if(devcmni->ware == SOFTWARE) {
            if(devmdlr->speed >= DEVI2C_ULTRAFAST) {
                GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStructure.Pull = GPIO_NOPULL;
                GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
            } else {
                GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
                GPIO_InitStructure.Pull = GPIO_PULLUP;
                GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            }
            devcmniio->SCL_SCK.Init.Structure = GPIO_InitStructure;
            devcmniio->SCL_SCK.Init.State = DEVIO_PIN_SET;
            devcmniio->SDA_SDO_TXD_OWRE.Init.Structure = GPIO_InitStructure;
            devcmniio->SDA_SDO_TXD_OWRE.Init.State = DEVIO_PIN_SET;
            DEVIO_Init(&devcmniio->SCL_SCK);             //初始化SCL
            DEVIO_Init(&devcmniio->SDA_SDO_TXD_OWRE);    //初始化SDA
        } else if(devcmni->ware == HARDWARE) {
            //hal库的硬件I2C初始化,待补充
        }
    } else if(devcmni->protocol == SPI) {
        if(devcmni->ware == SOFTWARE) {
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStructure.Pull = GPIO_NOPULL;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
            devcmniio->SCL_SCK.Init.Structure = GPIO_InitStructure;
            devcmniio->SCL_SCK.Init.State = DEVIO_PIN_SET;
            DEVIO_Init(&devcmniio->SCL_SCK);    //初始化SCK
            if(!isDevIoNull(&devcmniio->SDA_SDO_TXD_OWRE)) {
                devcmniio->SDA_SDO_TXD_OWRE.Init.Structure = GPIO_InitStructure;
                devcmniio->SDA_SDO_TXD_OWRE.Init.State = DEVIO_PIN_SET;
                DEVIO_Init(&devcmniio->SDA_SDO_TXD_OWRE);    //初始化SDI_RXD
            }
            if(!isDevIoNull(&devcmniio->SDI_RXD)) {
                GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
                GPIO_InitStructure.Pull = GPIO_PULLUP;
                GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
                devcmniio->SDI_RXD.Init.Structure = GPIO_InitStructure;
                devcmniio->SDI_RXD.Init.State = DEVIO_PIN_SET;
                DEVIO_Init(&devcmniio->SDI_RXD);    //初始化SDO
            }
            if(!isDevIoNull(&devcmniio->CS)) {
                GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
                GPIO_InitStructure.Pull = GPIO_NOPULL;
                GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
                devcmniio->CS.Init.Structure = GPIO_InitStructure;
                devcmniio->CS.Init.State = DEVIO_PIN_SET;
                DEVIO_Init(&devcmniio->CS);    //初始化CS
            }
        } else if(devcmni->ware == HARDWARE) {
            //hal库的硬件SPI初始化,待补充
        }
    } else if(devcmni->protocol == USART) {
        //...
    } else if(devcmni->protocol == ONEWIRE) {
        if(devcmni->ware == SOFTWARE) {
            GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
            GPIO_InitStructure.Pull = GPIO_PULLUP;
            GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            devcmniio->SDA_SDO_TXD_OWRE.Init.Structure = GPIO_InitStructure;
            devcmniio->SDA_SDO_TXD_OWRE.Init.State = DEVIO_PIN_SET;
            DEVIO_Init(&devcmniio->SDA_SDO_TXD_OWRE);    //初始化OWIO
        }
    }
#elif defined(STM32FWLIB)
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    if(devcmni->protocol == I2C) {
        if(devcmni->ware == SOFTWARE) {
            I2C_ModuleHandleTypeDef *i2cdev = devcmni->modular;
            if(i2cdev->speed >= DEVI2C_ULTRAFAST) {
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            } else {
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            }
            devcmniio->SCL_SCK.Init.Structure = GPIO_InitStructure;
            devcmniio->SCL_SCK.Init.State = DEVIO_PIN_SET;
            devcmniio->SDA_SDO_TXD_OWRE.Init.Structure = GPIO_InitStructure;
            devcmniio->SDA_SDO_TXD_OWRE.Init.State = DEVIO_PIN_SET;
            DEVIO_Init(&devcmniio->SCL_SCK);             //初始化SCL
            DEVIO_Init(&devcmniio->SDA_SDO_TXD_OWRE);    //初始化SDA
        } else if(devcmni->ware == HARDWARE) {
            //固件库的硬件I2C初始化,待补充
        }
    } else if(devcmni->protocol == SPI) {
        if(devcmni->ware == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            devcmniio->SCL_SCK.Init.Structure = GPIO_InitStructure;
            devcmniio->SCL_SCK.Init.State = DEVIO_PIN_SET;
            DEVIO_Init(&devcmniio->SCL_SCK);    //初始化SCK
            if(!isDevIoNull(&devcmniio->SDA_SDO_TXD_OWRE)) {
                devcmniio->SDA_SDO_TXD_OWRE.Init.Structure = GPIO_InitStructure;
                devcmniio->SDA_SDO_TXD_OWRE.Init.State = DEVIO_PIN_SET;
                DEVIO_Init(&devcmniio->SDA_SDO_TXD_OWRE);    //初始化SDI_RXD
            }
            if(!isDevIoNull(&devcmniio->SDI_RXD)) {
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                devcmniio->SDI_RXD.Init.Structure = GPIO_InitStructure;
                devcmniio->SDI_RXD.Init.State = DEVIO_PIN_SET;
                DEVIO_Init(&devcmniio->SDI_RXD);    //初始化SDO
            }
            if(!isDevIoNull(&devcmniio->CS)) {
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                devcmniio->CS.Init.Structure = GPIO_InitStructure;
                devcmniio->CS.Init.State = DEVIO_PIN_SET;
                DEVIO_Init(&devcmniio->CS);    //初始化CS
            }
        } else if(devcmni->ware == HARDWARE) {
            //固件库的硬件SPI初始化,待补充
        }
    } else if(devcmni->protocol == USART) {
        //...
    } else if(devcmni->protocol == ONEWIRE) {
        if(devcmni->ware == SOFTWARE) {
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            devcmniio->SDA_SDO_TXD_OWRE.Init.Structure = GPIO_InitStructure;
            devcmniio->SDA_SDO_TXD_OWRE.Init.State = DEVIO_PIN_SET;
            DEVIO_Init(&devcmniio->SDA_SDO_TXD_OWRE);    //初始化OWIO
        }
    }
#endif
}
//    I2C/SPI/ONEWIRE通信驱动函数
bool DEVCMNI_ReadBit(uint8_t address) {
    bool bit = 0;
    DEVCMNI_TypeDef *devcmni = DEV_GetActDevCmni();
    void *handle = devcmni;
    if(devcmni->protocol == ONEWIRE) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVOWRE_SOFTWARE_ENABLED)
            DEVONEWIRE_ReadBit((ONEWIRE_ModuleHandleTypeDef *)handle, &bit);
#endif    // DEVOWRE_SOFTWARE_ENABLED
        }
    }
    return bit;
}
uint8_t DEVCMNI_ReadByte(uint8_t address) {
    uint8_t byte = 0;
    DEVCMNI_TypeDef *devcmni = DEV_GetActDevCmni();
    DEVCMNIIO_TypeDef *devio = DEV_GetActDevCmniIo();
    void *handle = devcmni;
    if(devcmni->protocol == I2C) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVI2C_SOFTWARE_ENABLED)
            DEVI2C_Transmit((I2C_ModuleHandleTypeDef *)handle, &byte, 1, address, 1, 0xff);
#endif    // DEVI2C_SOFTWARE_ENABLED
        } else if(devcmni->ware == HARDWARE) {
#if defined(DEVI2C_HARDWARE_ENABLED)
            DEVI2C_Transmit_H((I2C_ModuleHandleTypeDef *)handle, &byte, 1, address, 1, 0xff);
#endif    // DEVI2C_HARDWARE_ENABLED
        }
    } else if(devcmni->protocol == SPI) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVSPI_SOFTWARE_ENABLED)
            DEVSPI_Transmit((SPI_ModuleHandleTypeDef *)handle, &byte, 1, 1, 0xff);
#endif    // DEVSPI_SOFTWARE_ENABLED
        } else if(devcmni->ware == HARDWARE) {
#if defined(DEVSPI_HARDWARE_ENABLED)
            if(!((SPI_ModuleHandleTypeDef *)handle)->skip) {
                if(devio->CS.GPIOx != NULL) {
                    DEVIO_ResetPin(&devio->CS);
                }
            }
            DEVSPI_Transmit_H((SPI_ModuleHandleTypeDef *)handle, &byte, 1, 1, 0xff);
            if(!((SPI_ModuleHandleTypeDef *)handle)->skip) {
                if(devio->CS.GPIOx != NULL) {
                    DEVIO_SetPin(&devio->CS);
                }
            }
#endif    // DEVSPI_HARDWARE_ENABLED
        }
    } else if(devcmni->protocol == ONEWIRE) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVOWRE_SOFTWARE_ENABLED)
            DEVONEWIRE_Read((ONEWIRE_ModuleHandleTypeDef *)handle, &byte, 1);
#endif    // DEVOWRE_SOFTWARE_ENABLED
        }
    }
    return byte;
}
void DEVCMNI_WriteByte(uint8_t byte, uint8_t address) {
    DEVCMNI_TypeDef *devcmni = DEV_GetActDevCmni();
    DEVCMNIIO_TypeDef *devio = DEV_GetActDevCmniIo();
    void *handle = devcmni;
    if(devcmni->protocol == I2C) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVI2C_SOFTWARE_ENABLED)
            DEVI2C_Transmit((I2C_ModuleHandleTypeDef *)handle, &byte, 1, address, 0, 0xff);
#endif    // DEVI2C_SOFTWARE_ENABLED
        } else if(devcmni->ware == HARDWARE) {
#if defined(DEVI2C_HARDWARE_ENABLED)
            DEVI2C_Transmit_H((I2C_ModuleHandleTypeDef *)handle, &byte, 1, address, 0, 0xff);
#endif    // DEVI2C_HARDWARE_ENABLED
        }
    } else if(devcmni->protocol == SPI) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVSPI_SOFTWARE_ENABLED)
            DEVSPI_Transmit((SPI_ModuleHandleTypeDef *)handle, &byte, 1, 0, 0xff);
#endif    // DEVSPI_SOFTWARE_ENABLED
        } else if(devcmni->ware == HARDWARE) {
#if defined(DEVSPI_HARDWARE_ENABLED)
            if(!((SPI_ModuleHandleTypeDef *)handle)->skip) {
                if(devio->CS.GPIOx != NULL) {
                    DEVIO_ResetPin(&devio->CS);
                }
            }
            DEVSPI_Transmit_H((SPI_ModuleHandleTypeDef *)handle, &byte, 1, 0, 0xff);
            if(!((SPI_ModuleHandleTypeDef *)handle)->skip) {
                if(devio->CS.GPIOx != NULL) {
                    DEVIO_SetPin(&devio->CS);
                }
            }
#endif    // DEVSPI_HARDWARE_ENABLED
        }
    } else if(devcmni->protocol == ONEWIRE) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVOWRE_SOFTWARE_ENABLED)
            DEVONEWIRE_Write((ONEWIRE_ModuleHandleTypeDef *)handle, &byte, 1, 0xff);
#endif    // DEVOWRE_SOFTWARE_ENABLED
        }
    }
}
DEV_StatusTypeDef DEVCMNI_Read(uint8_t *pdata, size_t size, size_t *length, uint8_t address) {
    DEVCMNI_TypeDef *devcmni = DEV_GetActDevCmni();
    DEVCMNIIO_TypeDef *devio = DEV_GetActDevCmniIo();
    void *handle = devcmni;
    DEV_StatusTypeDef res;
    if(devcmni->protocol == I2C) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVI2C_SOFTWARE_ENABLED)
            res = DEVI2C_Transmit((I2C_ModuleHandleTypeDef *)handle, pdata, size, address, 1, 0xff);
#endif    // DEVI2C_SOFTWARE_ENABLED
        } else if(devcmni->ware == HARDWARE) {
#if defined(DEVI2C_HARDWARE_ENABLED)
            res = DEVI2C_Transmit_H((I2C_ModuleHandleTypeDef *)handle, pdata, size, address, 1, 0xff);
#endif    // DEVI2C_HARDWARE_ENABLED
        }
    } else if(devcmni->protocol == SPI) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVSPI_SOFTWARE_ENABLED)
            res = DEVSPI_Transmit(((SPI_ModuleHandleTypeDef *)handle), pdata, size, 1, 0xff);
#endif    // DEVSPI_SOFTWARE_ENABLED
        } else if(devcmni->ware == HARDWARE) {
#if defined(DEVSPI_HARDWARE_ENABLED)
            if(!((SPI_ModuleHandleTypeDef *)handle)->skip) {
                if(devio->CS.GPIOx != NULL) {
                    DEVIO_ResetPin(&devio->CS);
                }
            }
            res = DEVSPI_Transmit_H(((SPI_ModuleHandleTypeDef *)handle), pdata, size, 1, 0xff);
            if(!((SPI_ModuleHandleTypeDef *)handle)->skip) {
                if(devio->CS.GPIOx != NULL) {
                    DEVIO_SetPin(&devio->CS);
                }
            }
#endif    // DEVSPI_HARDWARE_ENABLED
        }
    } else if(devcmni->protocol == USART) {
        if(devcmni->ware == HARDWARE) {
#if defined(DEVUART_HARDWARE_ENABLED)
            res = DEVUART_Receive((UART_ModuleHandleTypeDef *)handle, pdata, size, length);
#endif    // DEVUART_HARDWARE_ENABLED
        }
    } else if(devcmni->protocol == ONEWIRE) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVOWRE_SOFTWARE_ENABLED)
            res = DEVONEWIRE_Read((ONEWIRE_ModuleHandleTypeDef *)handle, pdata, size);
#endif    // DEVOWRE_SOFTWARE_ENABLED
        }
    }
    return res;
}
DEV_StatusTypeDef DEVCMNI_Write(uint8_t *pdata, size_t size, uint8_t address) {
    DEVCMNI_TypeDef *devcmni = DEV_GetActDevCmni();
    DEVCMNIIO_TypeDef *devio = DEV_GetActDevCmniIo();
    void *handle = devcmni;
    DEV_StatusTypeDef res;
    if(devcmni->protocol == I2C) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVI2C_SOFTWARE_ENABLED)
            res = DEVI2C_Transmit((I2C_ModuleHandleTypeDef *)handle, pdata, size, address, 0, 0xff);
#endif    // DEVI2C_SOFTWARE_ENABLED
        } else if(devcmni->ware == HARDWARE) {
#if defined(DEVI2C_HARDWARE_ENABLED)
            res = DEVI2C_Transmit_H((I2C_ModuleHandleTypeDef *)handle, pdata, size, address, 0, 0xff);
#endif    // DEVI2C_HARDWARE_ENABLED
        }
    } else if(devcmni->protocol == SPI) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVSPI_SOFTWARE_ENABLED)
            res = DEVSPI_Transmit(((SPI_ModuleHandleTypeDef *)handle), pdata, size, 0, 0xff);
#endif    // DEVSPI_SOFTWARE_ENABLED
        } else if(devcmni->ware == HARDWARE) {
#if defined(DEVSPI_HARDWARE_ENABLED)
            if(!((SPI_ModuleHandleTypeDef *)handle)->skip) {
                if(devio->CS.GPIOx != NULL) {
                    DEVIO_ResetPin(&devio->CS);
                }
            }
            res = DEVSPI_Transmit_H(((SPI_ModuleHandleTypeDef *)handle), pdata, size, 0, 0xff);
            if(!((SPI_ModuleHandleTypeDef *)handle)->skip) {
                if(devio->CS.GPIOx != NULL) {
                    DEVIO_SetPin(&devio->CS);
                }
            }
#endif    // DEVSPI_HARDWARE_ENABLED
        }
    } else if(devcmni->protocol == USART) {
        if(devcmni->ware == HARDWARE) {
#if defined(DEVUART_HARDWARE_ENABLED)
            res = DEVUART_Transmit((UART_ModuleHandleTypeDef *)handle, pdata, size);
#endif    // DEVUART_HARDWARE_ENABLED
        }
    } else if(devcmni->protocol == ONEWIRE) {
        if(devcmni->ware == SOFTWARE) {
#if defined(DEVOWRE_SOFTWARE_ENABLED)
            res = DEVONEWIRE_Write((ONEWIRE_ModuleHandleTypeDef *)handle, pdata, size, 0xff);
#endif    // DEVOWRE_SOFTWARE_ENABLED
        }
    }
    return res;
}


/*****   设备构造&错误处理部分   *****/
static bool isDevNull(DEV_TypeDef *dev) {
    if(dev == NULL || dev == NULL) {
        return true;
    }
    return false;
}
static bool isDevsNull(DEVS_TypeDef *devs, DEV_TypeDef dev[]) {
    if(devs == NULL) {
        return true;
    }
    for(poolsize i = 0; i < devs->size; i++) {
        if(isDevNull(&dev[i])) {
            return true;
        }
    }
    return false;
}
static inline bool isDevIoNull(DEVIO_TypeDef *devio) {
    //todo: 模仿hal库, 改为使用assert_param进行有效性判断
#if defined(STM32)
    if(devio->GPIOx == NULL) {
        return true;
    }
#if defined(USE_LLLIB) && defined(USE_FULL_LL_DRIVER)
    if(devio->GPIO_Pin == (uint16_t)0x0000) {
        return true;
    }
#else
    if(devio->GPIO_Pin == (uint32_t)(0x00000000)) {
        return true;
    }
#endif
#endif
    return false;
}
static bool isDevCmniNull(DEVCMNI_TypeDef *devcmni) {
    if(devcmni->protocol == 0 || devcmni->ware == 0) {    //若设备通信配置未正确配置, 则返回
        return true;
    }
    return false;
}
static bool isDevCmniBusNull(DEVCMNI_TypeDef *devcmni) {
    if(devcmni->bus == NULL) {    //若总线句柄为空, 则返回
        return true;
    }
    return false;
}
static bool isDevCmniIoNull(DEVCMNI_TypeDef *devcmni, DEVCMNIIO_TypeDef *devcmniio) {
    if(devcmni->protocol == I2C) {
        if(isDevIoNull(&devcmniio->SCL_SCK) || isDevIoNull(&devcmniio->SDA_SDO_TXD_OWRE)) {
            return true;
        }
    } else if(devcmni->protocol == SPI) {
        if(isDevIoNull(&devcmniio->SCL_SCK) || (isDevIoNull(&devcmniio->SDA_SDO_TXD_OWRE) && isDevIoNull(&devcmniio->SDI_RXD))) {
            return true;
        }
    } else if(devcmni->protocol == ONEWIRE) {
        if(isDevIoNull(&devcmniio->SDA_SDO_TXD_OWRE)) {
            return true;
        }
    }
    return false;
}
/**
 * @description: 初始化一个设备类, 从设备池中分配空闲指针指向设备数组, 并将序号更新至设备类句柄中
 * @param {DEVS_TypeDef} *devs
 * @param {DEV_TypeDef} dev
 * @param {poolsize} num
 * @return {*}
 */
static bool initDevPool(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize size) {
    if(devs->type == DEVUNDEF || _devSize + size >= DEVPOOL_MAXNUM) {
        return true;    //申请失败
    }
    devs->stream = 0;
    devs->pool = _devSize;
    devs->size += size;
    _devSize += size;
    for(poolsize i = 0; i < size; i++) {
        dev[i].type = devs->type;
        dev[i].state = 0;
        dev[i].error = 0;
        _devPool[devs->pool + i] = &dev[i];
    }
    return false;
}

/**
 * @description:
 * @param {int8_t} err
 * @return {*}
 */
void DEV_Error(uint16_t err) {
    DEVS_TypeDef *actdevs = DEV_GetActDevs();
    DEV_TypeDef *actdev = DEV_GetActDev();
    poolsize stream = actdevs->stream;

    printf("[ERROR!!!] %d\r\n", err ^ (actdevs->type << 8));
    if(err == DEVIO_NOFOUND) {
        return;
    } else if(err < 10) {
        while(1)
            ;
    } else if(err >= 0x0100) {
        DEV_SetActStream(actdevs, stream);
        actdev->error = 1;
    }
}
void DEV_ioConfigur(void) {
    if(_actDev->io.init != NULL) {
        _actDev->io.init();
    }
}
void DEV_ioInit(void) {
    for(poolsize i = sizeof(DEVCMNIIO_TypeDef) / sizeof(DEVIO_TypeDef); i < DEV_GetActDev()->io.num; i++) {
        if(isDevIoNull(&DEV_GetActDevIo()[i])) {
            DEV_Error(DEVIO_NOFOUND);
            continue;
        }
        DEVIO_Init(&DEV_GetActDevIo()[i]);
    }
}

// #define DEVCMNI_MAXNUM     20     //设备总线数量
// static poolsize _bussize = 0;
// static DEVS_TypeDef _bus = {.type = CMNIBUS};
// static DEV_TypeDef _buspool[DEVCMNI_MAXNUM] = {0};
// /**
//  * @description: 从总线池中分配空间保存总线数组, 并将序号更新至总线变量中
//  * @param {DEV_TypeDef} dev
//  * @param {poolsize} num
//  * @return {*}
//  */
// bool BUSPOOL_Init(DEV_TypeDef dev[], poolsize num) {
//     if(_bussize + num >= DEVCMNI_MAXNUM) {
//         return true;    //申请失败
//     }
//     for(poolsize i = 0; i < num; i++) {
//         _buspool[_bussize + i] = dev[i];
//     }
//     _bussize += num;
//     return false;
// }
void DEV_cmniConfigur(void) {
    DEVCMNI_TypeDef *devcmni = DEV_GetActDevCmni();
    DEVCMNIIO_TypeDef *devio = DEV_GetActDevCmniIo();
    // poolsize num = 0;
    for(poolsize i = 0; i < DEV_GetActDev()->cmni.num; i++) {
        if(isDevCmniNull(&devcmni[i]) || isDevCmniBusNull(&devcmni[i])) {    //检查通信配置有效性
            DEV_Error(DEVCMNI_NOFOUND);
            devcmni[i].bus = NULL;
            continue;
        }
        if(isDevCmniIoNull(&devcmni[i], &devio[i])) {    //检查通信IO有效性
            DEV_Error(DEVCMNIIO_NOFOUND);
            devcmni[i].bus = NULL;
            continue;
        }
        // DEV_TypeDef bus = {.io = {.num = sizeof(DEVCMNIIO_TypeDef) / sizeof(DEVIO_TypeDef), .confi = DEV_GetActDevIo()},
        //                    .cmni = {.num = 1, .confi = devcmni[i].bus}};
        // if(BUSPOOL_Init(&bus, 1)) {
        //     DEV_Error(DEV_POOLFULL);
        // }
        DEVCMNI_BusInit(&devcmni[i]);
        // num++;
    }
    // if(initDevPool(&_bus, &_buspool[_bussize], num)) {
    //     DEV_Error(DEV_POOLFULL);
    // }
}
void DEV_cmniInit(void) {
    DEVCMNI_TypeDef *devcmni = DEV_GetActDevCmni();
    DEVCMNIIO_TypeDef *devio = DEV_GetActDevCmniIo();
    for(poolsize i = 0; i < DEV_GetActDev()->cmni.num; i++) {
        if(isDevCmniNull(&devcmni[i]) || isDevCmniBusNull(&devcmni[i])) {    //检查通信配置有效性
            DEV_Error(DEVCMNI_NOFOUND);
            devcmni[i].bus = NULL;
            continue;
        }
        if(isDevCmniIoNull(&devcmni[i], &devio[i])) {    //检查通信IO有效性
            DEV_Error(DEVCMNIIO_NOFOUND);
            devcmni[i].bus = NULL;
            continue;
        }
        DEVCMNI_Init(&devcmni[i], &devio[i]);
    }
}
void DEV_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize size) {
    if(isDevsNull(devs, dev)) {
        DEV_Error(DEV_NOFOUND);
    }
    if(initDevPool(devs, dev, size)) {    //初始化一类设备实例到设备池中
        DEV_Error(DEV_POOLFULL);
    }
    DEV_DoAction(devs, DEV_ioConfigur);
    DEV_DoAction(devs, DEV_cmniConfigur);
    DEV_DoAction(devs, DEV_ioInit);      //设备IO初始化
    DEV_DoAction(devs, DEV_cmniInit);    //设备通信初始化
}
