#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H
#include "oledlib.h"
typedef enum {
    OLED_SSD1306,
    OLED_SH1106
} OledChip_TypeDef;
typedef enum {    //常用的OLED地址为0111100和0111101
    OLED_I2CADDR1 = 0x3C,
    OLED_I2CADDR2 = 0x3D
} OledI2CAddr_TypeDef;

typedef struct {
    DEVCMNIIO_TypeDef cmniio;
    DEVIO_TypeDef DC;
    DEVIO_TypeDef RST;
} OLED_IOTypeDef;
typedef struct {
    DEVCMNI_TypeDef cmni;
} OLED_CMNITypeDef;
typedef struct {
    OledChip_TypeDef chip;
    bool flip;
} OLED_PARTypeDef;

extern char *oled_va_buf;
extern size_t oled_bufSize;

void OLED_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize uSize, char *buf, size_t bSize);
void OLED_Error(void);
void OLED_DevInit(uint8_t flip);

void OLED_On(void);
void OLED_Off(void);
void OLED_Reset(void);
void OLED_Flip(int8_t, int8_t);
void OLED_Clear(void);

void OLED_SetCursor(uint8_t page, uint8_t col);
void OLED_Fill(uint8_t (*Buffer)[SCREEN_PAGE][SCREEN_COLUMN]);
void OLED_FillByte(uint8_t, uint8_t, uint8_t);

#endif
