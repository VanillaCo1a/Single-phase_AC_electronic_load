#ifndef __PROTOCOL_HARDWARE_H
#define __PROTOCOL_HARDWARE_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buf;
    volatile size_t size;
    volatile size_t count;
    volatile bool state;
} BufferTypedef;
typedef struct {
    BufferTypedef receive;
    BufferTypedef transmit;
    bool usedma;
    bool checkidle;
    void *bus;
} UART_ModuleHandleTypeDef;


/***  HARDWARE IMPLEMENTATION FUNCTION OF I2C DEVICE COMMUNITCATION  ***/
#if defined(DEVI2C_HARDWARE_ENABLED)
#include "i2c.h"
DEVCMNI_StatusTypeDef DEVI2C_Transmit_H(I2C_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, uint8_t address, bool rw, uint32_t timeout);

#endif    // DEVI2C_HARDWARE_ENABLED


/***  HARDWARE IMPLEMENTATION FUNCTION OF SPI DEVICE COMMUNITCATION  ***/
#if defined(DEVSPI_HARDWARE_ENABLED)
#include "spi.h"
DEVCMNI_StatusTypeDef DEVSPI_Transmit_H(SPI_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, bool rw, uint32_t timeout);

#endif    // DEVSPI_HARDWARE_ENABLED


/***  HARDWARE IMPLEMENTATION FUNCTION OF 1-WIRE DEVICE COMMUNITCATION  ***/
#if defined(DEVOWRE_HARDWARE_ENABLED)
#include "onewire.h"

#endif    // DEVOWRE_HARDWARE_ENABLED


/***  HARDWARE IMPLEMENTATION FUNCTION OF UART DEVICE COMMUNITCATION  ***/
#if defined(DEVUART_HARDWARE_ENABLED)
#include "usart.h"
/* 串口接收函数 */
DEVCMNI_StatusTypeDef DEVUART_Receive(UART_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, size_t *length);
/* 串口发送函数 */
DEVCMNI_StatusTypeDef DEVUART_Transmit(UART_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size);

#endif    // DEVUART_HARDWARE_ENABLED

#endif    // !__PROTOCOL_HARDWARE_H
