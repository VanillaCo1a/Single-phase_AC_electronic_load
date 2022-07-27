#ifndef __OLED_FONT_BMP_H
#define __OLED_FONT_BMP_H
#include "oledlib.h"

struct Cn16CharTypeDef {            //汉字字模数据结构
    unsigned char  Index[3];        //汉字内码索引,一个汉字占两个字节
    unsigned char  Msk[32];         //点阵码数据(16*16有32个数据) 
};

extern const unsigned char Font_5x7[];
extern const unsigned char Font_8x16[];
extern struct Cn16CharTypeDef const CN16CHAR[];

extern const unsigned char BmpTest1[];
extern const unsigned char BmpTest2[];
extern const unsigned char BmpTest3[];
extern const unsigned char Like[];
extern const unsigned char Panda[];

#endif
