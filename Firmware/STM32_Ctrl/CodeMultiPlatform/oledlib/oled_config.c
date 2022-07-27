/***    原作者哔哩哔哩:                            MjGame         https://space.bilibili.com/38673747
            同GifHub:                                maoyongjie     https://github.com/hello-myj/stm32_oled/
            代码整理注释删减增加优化等 哔哩哔哩:    一只程序缘    https://space.bilibili.com/237304109
            整理之前的原代码随本代码一同提供,浏览以上网址获取更多相关信息,本代码以征得原作同意,感谢原作
    
        此c文件用于配置oled
        注意!!! 
        如需演示动画效果 OledTimeMsFunc()需放入1ms中断等中1ms调用一次为库提供时间基准 原作是放入滴答定时器中

        对颜色进行操作
        在这里主要是画线线条(一般为白)和填充(一般为白)的颜色
        (在oled.font中还有一个字体背景的颜色(一般为黑))
        在划线画图和打点时都会访问此文件函数获取打点的颜色                
    
        对字体进行操作
        设置字体背景颜色(字体颜色同划线线条颜色getLineColor())
        字体尺寸            ***/
            
#include "oled_config.h"

////////////////////////////////////////////////////////////////////////////
//如有需要还可以自己迭代字体颜色 文字背景颜色等
static Type_color _lineColor = pix_white;
static Type_color _fillColor = pix_white;

//设置/获取画线的像素点颜色
void setLineColor(Type_color value) {
    _lineColor = value;
}
Type_color getLineColor(void) {
    return _lineColor;
}
//设置/获取填充图形的像素点颜色
void setFillcolor(Type_color value) {
    _fillColor = value;
}
Type_color getFillColor(void) {
    return _fillColor;
}
//得到某个点的颜色
Type_color getPixel(int x, int y) {
    if(getPointBuffer(x,y)) {
        return pix_white;
    }else {
        return pix_black;
    }
}


////////////////////////////////////////////////////////////////////////////
static Type_textbk _TextBk = TEXT_BK_NULL;
static unsigned char _FontSize = 0;            //默认字体尺寸 为8x6

//设置字体填充的背景颜色： 1白,0黑 
//TEXT_BK_NULL：无背景, TEXT_BK_NOT_NULL：有背景
void SetTextBkMode(Type_textbk value) {
    _TextBk = value;
}
Type_textbk GetTextBkMode(void) {
    return _TextBk;
}
//在显示字体前先设置需要显示的字体的尺寸
//尺寸有 0 1 2 3
//对应像素是 0 8 16 24
//0为默认字体Font_8x16[], 8x16
//1为原作者字体Font_5x7[], 6x8
//2为1的2倍放大, 12x16
//3为1的3倍放大, 18x24
void SetFontSize(unsigned char value) {
    _FontSize = value;
}
unsigned char GetFontSize(void) {
    return _FontSize;
}


////////////////////////////////////////////////////////////////////////////
uint32_t OledTimeMs = 0;                            //时间基准

//请将此函数放入1ms中断里，为图形提供时基
//系统时间基准主要用于FrameRateUpdateScreen()中固定帧率刷新屏幕
void OledTimeMsFunc(void) {
    if(OledTimeMs != 0x00) { 
        OledTimeMs--;
    }
}
