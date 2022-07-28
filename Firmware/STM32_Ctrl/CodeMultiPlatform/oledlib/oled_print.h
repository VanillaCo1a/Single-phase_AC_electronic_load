#ifndef __OLED_PRINT_H
#define __OLED_PRINT_H
#include "oledlib.h"

//绘制颜色，简单绘制or复杂绘制
#define COLOR_CHOOSE_DRAW (SIMPLE_DRAW)
//设置/获取画线的像素点颜色
void setLineColor(Type_color value);
Type_color getLineColor(void);
//设置/获取填充图形的像素点颜色
void setFillcolor(Type_color value);
Type_color getFillColor(void);
//得到某个点的颜色
Type_color getPixel(int x,int y);
//设置/获取背景模式, 0为透明,1为正常
void SetTextBkMode(Type_textbk);
Type_textbk GetTextBkMode(void);
//设置/获取字体格式
void SetFontSize(unsigned char);
unsigned char GetFontSize(void);

void OledTimeMsFunc(void);



int GetY(void);
int GetX(void);
TypeXY GetXY(void);
void MoveTo(int x,int y);
void LineTo(int x,int y);

void DrawPixel(int x,int y);
void DrawLine(int x1,int y1,int x2,int y2);
void DrawFastHLine(int x, int y, unsigned char w);
void DrawFastVLine(int x, int y, unsigned char h);
void DrawPolyLineTo(const TypeXY *points,int num);
void DrawRect1(int left,int top,int right,int bottom);
void DrawFillRect1(int left,int top,int right,int bottom);
void DrawRect2(int left,int top,int width,int height);
void DrawFillRect2(int left,int top,int width,int height);
void DrawCircle( int usX_Center, int usY_Center, int usRadius);
void DrawFillCircle( int usX_Center, int usY_Center, int usRadius);
void DrawCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername);
void DrawFillCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername, int delta);
void DrawArc(int x,int y,unsigned char r,int angle_start,int angle_end);
void DrawFillArc(int x,int y,unsigned char r,int angle_start,int angle_end);
void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);
void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r);
void DrawEllipse(int x0,int y0,int a,int b);
void DrawFillEllipse(int x0,int y0,int a,int b);
void DrawEllipseRect( int x0, int y0, int x1, int y1);
void DrawTriangle(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2);
void DrawFillTriangle(int x0, int y0, int x1, int y1, int x2, int y2);



void OLED_PrintChar(uint8_t y, uint8_t x, char c);
void OLED_PrintString(uint8_t y, uint8_t x, char *str);
void OLED_PrintChinese(uint8_t y, uint8_t x, char *cn);
void OLED_PrintNum(uint8_t y, uint8_t x, uint32_t num, uint8_t len);
void OLED_PrintIntNum(uint8_t y, uint8_t x, int32_t num, uint8_t len);
void OLED_Printf(uint8_t x, uint8_t y, const char *str, ...);
void OLED_PrintBitmap(int32_t y, int32_t x, uint8_t w, uint8_t h, const uint8_t *bitmap);
int32_t OLED_PrintfDebug(const char *str, ...);

#endif
