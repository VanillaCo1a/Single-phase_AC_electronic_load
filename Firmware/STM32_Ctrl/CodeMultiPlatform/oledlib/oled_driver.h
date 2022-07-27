#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H
#include "oledlib.h"
#define OLED_NUM 2
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
} OLED_PARTypeDef;

void OLED_Confi(void);
void OLED_Error(void);
void OLED_DevInit(uint8_t flip);
void OLED_On(void);
void OLED_Off(void);
void OLED_Flip(int8_t, int8_t);
void OLED_FillScreen(uint8_t (*Buffer)[SCREEN_PAGE][SCREEN_COLUMN]);
void OLED_DirectByte(uint8_t, uint8_t, uint8_t);
void OLED_ClearScreen(void);

#endif
