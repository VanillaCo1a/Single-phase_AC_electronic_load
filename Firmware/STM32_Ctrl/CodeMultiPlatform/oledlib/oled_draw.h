#ifndef __OLED_DRAW_H
#define __OLED_DRAW_H
#include "oledlib.h"

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

#endif
