////////////////////////////////////////////////////////////////////////////
//*         为了做这个绘制图形库，我参考了很多其他的算法,花了大量时间去做移植
//*优化，其中参考最多的是github中的arduboy游戏机，大部分图形是移植它的，
//*现在是最初的初稿版本,还有很多地方需要优化改进。我想要将这个图形库做大，
//*后续会加入更多有趣的东西，和模块，代码都是用最简单的方式编写的，都是开源的。
//*后续也会加上注释说明，让更多人以单片机和oled屏来入门硬件编程，如果你
//使用了该库，请标明出处。b站关注oximeterg，可获取该库的最新版本和消息。
//注意：绘制填充或实心的图形，影响刷新效率（待优化中）
//*oled屏图形库0.0.0.1                                    作者：maoyongjie     
//https://github.com/hello-myj/stm32_oled/
//************************************************************************
//-------------------------------------------------------------------------         
/*                          x(0~127)                            //坐标说明
                         ------------------>
                        |
                        |y(0~63)
                        |
                        v                                                 */
//--------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
#ifndef __OLED_LIB_H
#define __OLED_LIB_H

//定义屏幕基本状况
//保持了可拓展性 如果换用其他类型屏
//因注意其他配套c文件中用了这些宏定义的部分
#define OLED_12864                      //屏幕类型
#define SCREEN_PART (2)                 //定义几个屏幕缓冲区, 每个缓冲区均为1个屏幕大小
#define SCREEN_PAGE (8)                 //总页数 (大行)
#define SCREEN_ROW (64)                 //总行数
#define SCREEN_COLUMN (128)             //总列数

//图形库的一些枚举类型
typedef enum {
    TEXT_BK_NULL = 0,
    TEXT_BK_NOT_NULL,
}Type_textbk;
typedef enum {
    pix_black,
    pix_white,
    //............
}Type_color;
typedef struct COORDINATE {
    int x;
    int y;
}TypeXY;
typedef struct ROATE {
    TypeXY center;
    float angle;
    int direct;
}TypeRoate;

#include "device.h"
#include "oled_driver.h"
#include "oled_buffer.h"
#include "oled_basic.h"
#include "oled_print.h"
#include "oled_font&bmp.h"
#endif
