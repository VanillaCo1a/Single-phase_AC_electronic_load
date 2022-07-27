#ifndef __OLED_BASIC_H
#define __OLED_BASIC_H
#include "oledlib.h"

#define point TypeXY
#define RADIAN(angle)  ((angle==0)?0:(3.14159f*angle/180))
#define MAX(x,y)          ((x)>(y)? (x):(y))
#define MIN(x,y)          ((x)<(y)? (x):(y))
#define SWAP(x, y) \
    (y) = (x) + (y); \
    (x) = (y) - (x); \
    (y) = (y) - (x);

//#define HW_IIC    (0)
//#define SW_IIC    (1)
//#define HW_SPI    (2)
//#define SW_SPI    (3)
//#define HW_8080   (4)
//#define IIC_1     (10)
//#define IIC_2     (11)
//#define SPI_1     (20)
//#define SPI_2     (21)

void OLED_clearBuffer(void);
void OLED_updateScreen(void);

//设置背景模式，0为透明，1为正常。常
void SetTextBkMode(Type_textbk);
//获取当前背景模式
Type_textbk GetTextBkMode(void);
void FloodFill(unsigned char x,unsigned char y,int oldcolor,int newcolor);
void FillRect(int x,int y,int width,int height);
void FloodFill2(unsigned char x,unsigned char y,int oldcolor,int newcolor);
void FillVerticalLine(int x,int y,int height,int value);
void FillByte(int page,int x,unsigned  char byte);
void SetRotateValue(int x,int y,float angle,int direct);
void SetRotateCenter(int x0,int y0);
void SetAngle(float angle);
void SetAnggleDir(int direction);
TypeXY GetRotateXY(int x,int y);

unsigned char FrameRateUpdateScreen(int value);
#endif
