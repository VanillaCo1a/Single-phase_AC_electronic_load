#ifndef __OLED_CONFIG_H
#define __OLED_CONFIG_H
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
#endif
