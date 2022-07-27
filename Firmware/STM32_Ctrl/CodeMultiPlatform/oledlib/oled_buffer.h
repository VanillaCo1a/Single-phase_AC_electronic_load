#ifndef __OLED_BUFFER_H
#define __OLED_BUFFER_H
#include "oledlib.h"
#define SCREEN_BUFFER 1
#define TEMP_BUFFER 0

typedef enum {
    BUFF_COPY,
    BUFF_CLEAN,
    BUFF_COVER_L,
    BUFF_COVER_H
}Type_tempbuff;

void setBufferPart(int8_t);
int8_t getBufferPart(void);
void clearBufferPart(int8_t, uint8_t);

void clearBuffer(uint8_t);
uint8_t readByteBuffer(int32_t, int32_t);
void writeByteBuffer(int32_t, int32_t, uint8_t);
void setPointBuffer(int32_t, int32_t, int32_t value);
int8_t getPointBuffer(int32_t, int32_t );
void funcBuffer(int32_t, int8_t, int8_t);
void updateBuffer(void);
#endif
