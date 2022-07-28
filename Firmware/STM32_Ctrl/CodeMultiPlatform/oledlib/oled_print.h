#ifndef __OLED_PRINT_H
#define __OLED_PRINT_H
#include "oledlib.h"

void OLED_PrintChar(uint8_t y, uint8_t x, char c);
void OLED_PrintString(uint8_t y, uint8_t x, char *str);
void OLED_PrintChinese(uint8_t y, uint8_t x, char *cn);
void OLED_PrintNum(uint8_t y, uint8_t x, uint32_t num, uint8_t len);
void OLED_PrintIntNum(uint8_t y, uint8_t x, int32_t num, uint8_t len);
void OLED_Printf(uint8_t x, uint8_t y, const char *str, ...);
void OLED_PrintBitmap(int32_t y, int32_t x, uint8_t w, uint8_t h, const uint8_t *bitmap);
int32_t OLED_PrintfDebug(const char *str, ...);

#endif
