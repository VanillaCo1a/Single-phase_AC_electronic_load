#include "device.h"
#include "device_protocol.h"
//tofix: ac5编译不支持__weak函数在别处定义为inline
__weak void DEVCMNI_SCL_Set(bool dir) {}
__weak void DEVCMNI_SDA_OWRE_Set(bool dir) {}
__weak void DEVCMNI_SCL_SCK_Out(bool pot) {}
__weak void DEVCMNI_SDA_SDI_OWRE_Out(bool pot) {}
__weak bool DEVCMNI_SCL_In(void) { return HIGH; }
__weak bool DEVCMNI_SDA_OWRE_In(void) { return HIGH; }
__weak bool DEVCMNI_SDO_In(void) { return HIGH; }
__weak void DEVCMNI_CS_Out(bool pot) {}
__weak void DEVCMNI_Error(int8_t err) {}
__weak void DEVCMNI_Delayus(uint64_t us) {}
__weak void DEVCMNI_Delayms(uint64_t ms) {}
__weak int8_t DEVCMNI_Delayus_paral(uint64_t us) { return 1; }


/***  HARDWARE IMPLEMENTATION FUNCTION OF I2C DEVICE COMMUNITCATION  ***/
#if defined(DEVI2C_HARDWARE_ENABLED)
DEVCMNI_StatusTypeDef DEVI2C_Transmit_H(
    I2C_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, uint8_t address, bool rw, uint32_t timeout) {
    DEVCMNI_StatusTypeDef res;
    if(rw) {
#if defined(STM32HAL)
#if defined(HAL_I2C_MODULE_ENABLED)
        res = (DEVCMNI_StatusTypeDef)HAL_I2C_Mem_Read(
            modular->bus, (modular->addr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, pdata, size, timeout);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
        res = (DEVCMNI_StatusTypeDef)HAL_FMPI2C_Mem_Read(
            modular->bus, (modular->addr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, pdata, size, timeout);
#endif    // HAL_I2C_MODULE_ENABLED | HAL_FMPI2C_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
        //固件库的硬件I2C驱动函数,待补充
#endif
    } else {
#if defined(STM32HAL)
#if defined(HAL_I2C_MODULE_ENABLED)
        res = (DEVCMNI_StatusTypeDef)HAL_I2C_Mem_Write(
            modular->bus, (modular->addr << 1) | 0X00, address, I2C_MEMADD_SIZE_8BIT, pdata, size, timeout);
#elif defined(HAL_FMPI2C_MODULE_ENABLED)
        res = (DEVCMNI_StatusTypeDef)HAL_FMPI2C_Mem_Write(
            modular->bus, (modular->addr << 1) | 0X00, address, FMPI2C_MEMADD_SIZE_8BIT, pdata, size, timeout);
#endif    // HAL_I2C_MODULE_ENABLED | HAL_FMPI2C_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
        //固件库的硬件I2C驱动函数,待补充
#endif
    }
    return res;
}

#endif    // DEVI2C_HARDWARE_ENABLED


/***  HARDWARE IMPLEMENTATION FUNCTION OF SPI DEVICE COMMUNITCATION  ***/
#if defined(DEVSPI_HARDWARE_ENABLED)
DEVCMNI_StatusTypeDef DEVSPI_Transmit_H(
    SPI_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, bool rw, uint32_t timeout) {
    DEVCMNI_StatusTypeDef res = false;
    if(modular->duplex == DEVSPI_FULL_DUPLEX) {
        //to add
    } else if(modular->duplex == DEVSPI_HALF_DUPLEX) {
        if(rw) {
#if defined(STM32HAL)
#if defined(HAL_SPI_MODULE_ENABLED)
            res = (DEVCMNI_StatusTypeDef)HAL_SPI_Receive(modular->bus, pdata, size, timeout);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif    // HAL_SPI_MODULE_ENABLED | HAL_QSPI_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#endif
        } else {
#if defined(STM32HAL)
#if defined(HAL_SPI_MODULE_ENABLED)
            res = (DEVCMNI_StatusTypeDef)HAL_SPI_Transmit(modular->bus, pdata, size, timeout);
#elif defined(HAL_QSPI_MODULE_ENABLED)
            //todo: SPI_Write for QSPI
#endif    // HAL_SPI_MODULE_ENABLED | HAL_QSPI_MODULE_ENABLED
#elif defined(STM32FWLIBF1)
            //固件库的硬件SPI驱动函数,待补充
#endif
        }
    }
    return res;
}

#endif    // DEVSPI_HARDWARE_ENABLED


/***  HARDWARE IMPLEMENTATION FUNCTION OF 1-WIRE DEVICE COMMUNITCATION  ***/
#if defined(DEVOWRE_HARDWARE_ENABLED)

#endif    // DEVOWRE_HARDWARE_ENABLED


/***  HARDWARE IMPLEMENTATION FUNCTION OF UART DEVICE COMMUNITCATION  ***/
#if defined(DEVUART_HARDWARE_ENABLED)
static UART_ModuleHandleTypeDef *uartmodular;
static void DEVUART_Init(UART_ModuleHandleTypeDef *modular) {
    uartmodular = modular;
}

static void DEVUART_ReceiveStart(void);
static void DEVUART_TransmitStart(void);
static void DEVUART_ReceiveEnd(UART_ModuleHandleTypeDef *muart, size_t count);
static void DEVUART_TransmitEnd(UART_ModuleHandleTypeDef *muart);
static UART_ModuleHandleTypeDef *DEVUART_GetModular(void *bus);

/* 串口接收函数 */
DEVCMNI_StatusTypeDef DEVUART_Receive(
    UART_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, size_t *length) {
    DEVUART_Init(modular);
    if(!uartmodular->receive.state) {
        uartmodular->receive.size = size;
        uartmodular->receive.buf = pdata;
        if(uartmodular->receive.size != 0 && uartmodular->receive.buf != NULL) {
            DEVUART_ReceiveStart();
        }
    } else {
        *length = uartmodular->receive.count;
        uartmodular->receive.count = 0;
        uartmodular->receive.state = false;
        return DEVCMNI_OK;
    }
    return DEVCMNI_BUSY;
}

/* 串口发送函数 */
DEVCMNI_StatusTypeDef DEVUART_Transmit(
    UART_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size) {
    DEVUART_Init(modular);
    if(uartmodular->transmit.state) {
        uartmodular->transmit.size = size;
        uartmodular->transmit.buf = pdata;
        if(uartmodular->transmit.size != 0 && uartmodular->transmit.buf != NULL) {
            DEVUART_TransmitStart();
        }
        uartmodular->transmit.state = false;
        return DEVCMNI_OK;
    }
    return DEVCMNI_BUSY;
}

/* 中断回调函数 */
#if defined(STM32)
#if defined(STM32HAL)
/* 指定空间数据接收完毕中断回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    UART_ModuleHandleTypeDef *muart = DEVUART_GetModular(huart);
    if(huart->RxState == HAL_UART_STATE_READY || huart->hdmarx->State == HAL_DMA_STATE_READY) {
        /* 判断句柄标志位, 是否为数据接收完毕 */
        DEVUART_ReceiveEnd(muart, muart->receive.size);
    }
}
/* 指定空间数据半接收/接收完毕/总线空闲时中断回调函数 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
    UART_ModuleHandleTypeDef *muart = DEVUART_GetModular(huart);
    if(huart->RxState == HAL_UART_STATE_READY || huart->hdmarx->State == HAL_DMA_STATE_READY) {
        /* 判断句柄标志位, 是否为数据接收完毕/总线空闲 */
        DEVUART_ReceiveEnd(muart, size);
    } else if(size == muart->receive.size / 2) {
        /* 其他情况则为数据半接收完毕, 稳妥起见再对接收长度进行一次判断 */
    }
}
/* 数据发送完毕中断回调函数 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    UART_ModuleHandleTypeDef *muart = DEVUART_GetModular(huart);
    DEVUART_TransmitEnd(muart);
}
#elif defined(STM32FWLIB)
/* 指定空间数据接收完毕中断回调函数 */
void FWLIB_UART_RxCpltCallback(USART_TypeDef *USARTx) {
    UART_ModuleHandleTypeDef *muart = DEVUART_GetModular(USARTx);
    DEVUART_ReceiveEnd(muart, muart->receive.size);
}
/* 指定空间数据接收完毕/总线空闲时中断回调函数 */
void FWLIB_UARTEx_RxEventCallback(USART_TypeDef *USARTx, uint16_t size) {
    UART_ModuleHandleTypeDef *muart = DEVUART_GetModular(USARTx);
    DEVUART_ReceiveEnd(muart, size);
}
/* 数据发送完毕中断回调函数 */
void FWLIB_UART_TxCpltCallback(USART_TypeDef *USARTx) {
    UART_ModuleHandleTypeDef *muart = DEVUART_GetModular(USARTx);
    DEVUART_TransmitEnd(muart);
}
#endif
#endif


/* 串口开始接收函数, 调用底层库函数 */
static void DEVUART_ReceiveStart(void) {
#if defined(STM32)
#if defined(STM32HAL)
#if defined(HAL_UART_MODULE_ENABLED)
    if(uartmodular->usedma) {
#if defined(HAL_DMA_MODULE_ENABLED)
        HAL_UARTEx_ReceiveToIdle_DMA(
            uartmodular->bus, uartmodular->receive.buf, uartmodular->receive.size);
#endif    // HAL_DMA_MODULE_ENABLED
    } else {
        HAL_UARTEx_ReceiveToIdle_IT(
            uartmodular->bus, uartmodular->receive.buf, uartmodular->receive.size);
    }
#endif    // HAL_UART_MODULE_ENABLED
#elif defined(STM32FWLIB)
    FWLIB_UARTEx_ReceiveToIdle_IT(
        uartmodular->bus, uartmodular->receive.buf, uartmodular->receive.size);
#endif
#endif
}

/* 串口开始发送函数, 调用底层库函数 */
static void DEVUART_TransmitStart(void) {
#if defined(STM32)
#if defined(STM32HAL)
#if defined(HAL_UART_MODULE_ENABLED)
    if(uartmodular->usedma) {
#if defined(HAL_DMA_MODULE_ENABLED)
        HAL_UART_Transmit_DMA(
            uartmodular->bus, uartmodular->transmit.buf, uartmodular->transmit.size);
#endif    // HAL_DMA_MODULE_ENABLED
    } else {
        HAL_UART_Transmit_IT(
            uartmodular->bus, uartmodular->transmit.buf, uartmodular->transmit.size);
    }
#endif    // HAL_UART_MODULE_ENABLED
#elif defined(STM32FWLIB)
    FWLIB_UART_Transmit_IT(
        uartmodular->bus, uartmodular->transmit.buf, uartmodular->transmit.size);
#endif
#endif
}

/* 串口接收完毕函数, 应放在回调函数中 */
static void DEVUART_ReceiveEnd(UART_ModuleHandleTypeDef *muart, size_t count) {
    muart->receive.state = true;
    muart->receive.count = count;
}

/* 串口发送完毕函数, 应放在回调函数中 */
static void DEVUART_TransmitEnd(UART_ModuleHandleTypeDef *muart) {
    muart->transmit.state = true;
}

/* 获取串口句柄对应通信句柄 */
extern DEVS_TypeDef *myuarts;
extern DEV_TypeDef myuart[];
static UART_ModuleHandleTypeDef *DEVUART_GetModular(void *bus) {
    for(size_t i = 0; i < myuarts->size; i++) {
        if(((UART_ModuleHandleTypeDef *)myuart[i].cmni.confi->modular)->bus == bus) {
            return myuart[i].cmni.confi->modular;
        }
    }
    return NULL;
}
#endif    // DEVUART_HARDWARE_ENABLED

