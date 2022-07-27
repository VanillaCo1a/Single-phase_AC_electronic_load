#ifndef __OLED_PRINT_H
#define __OLED_PRINT_H
#include "oledlib.h"

void OLED_Char(int32_t y, int32_t x, uint8_t c);
void OLED_String(int32_t y, int32_t x, char *str);
void OLED_Num(uint8_t y, uint8_t x, uint32_t num, uint8_t len);
void OLED_IntNum(uint8_t y, uint8_t x, int32_t num, uint8_t len);
void OLED_Chinese(uint8_t y, uint8_t x, uint8_t *cn);
void OLED_Bitmap(int32_t y, int32_t x, uint8_t w, uint8_t h, const uint8_t *bitmap);
int32_t OLED_Printf(const char *str,...);

#endif
