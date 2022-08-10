/***    原作者哔哩哔哩:                       MjGame         https://space.bilibili.com/38673747
        同GifHub:                            maoyongjie     https://github.com/hello-myj/stm32_oled/
        代码整理注释删减增加优化等 哔哩哔哩:    一只程序缘      https://space.bilibili.com/237304109
        整理之前的原代码随本代码一同提供,浏览以上网址获取更多相关信息,本代码以征得原作同意,感谢原作  ***/
    
/***    此c文件用于对屏幕数组进行操作
        数组包括屏幕缓冲和临时缓冲
        函数只会目前选择的缓冲进行操作
        目前选择的缓冲可通过函数设置上面两个缓冲之一
        
        然后就是提供读或写入取缓冲中的8位数据或1位数据的接口
        最后是更新缓冲数据到屏幕  ***/

#include "oled_buffer.h"

//定义屏幕缓冲区
uint8_t screenBuffer[SCREEN_PART][SCREEN_PAGE][SCREEN_COLUMN] = {0};    //屏幕缓冲
static int8_t _selectedPart = 0;    //当前选中的缓冲区

extern void UpdateTempBuffer(void);
extern void UpdatescreenBuffer(void);
//设置程序目前选中的缓冲区
void setBufferPart(int8_t SelectedBuffer) {
    _selectedPart = SelectedBuffer;
}
//获取程序目前选中的缓冲区
int8_t getBufferPart(void) {
    return _selectedPart;
}
//在输入的缓冲区中清除数据
void clearBufferPart(int8_t part, uint8_t val) {
    memset(screenBuffer[part],val,sizeof(screenBuffer[part]));
}


////////////////////////////////////////////////////////////////////////////
//在程序选中的缓冲区中清除数据
void clearBuffer(uint8_t val) {
    memset(screenBuffer[getBufferPart()],val,sizeof(screenBuffer[getBufferPart()]));
}
//从选中的缓冲区中读取8位数据
uint8_t readByteBuffer(int32_t page, int32_t x) {
    return screenBuffer[getBufferPart()][page][x];
}
//在选中的缓冲区中写入8位数据
void writeByteBuffer(int32_t page, int32_t x, uint8_t byte) {
    screenBuffer[getBufferPart()][page][x] = byte;
}
//设置选中缓冲区的某一点的亮灭
void setPointBuffer(int32_t x,int32_t y,int32_t value) {
    if(x>SCREEN_COLUMN-1 || y>SCREEN_ROW-1) {    //超出范围
        return;
    }
    if(value) {
        screenBuffer[getBufferPart()][y/SCREEN_PAGE][x] |= 1<<(y%SCREEN_PAGE);
    }else {
        screenBuffer[getBufferPart()][y/SCREEN_PAGE][x] &= ~(1<<(y%SCREEN_PAGE));
    }
}
//获取选中缓冲区的某一点的颜色
int8_t getPointBuffer(int32_t x, int32_t y) {
    if(x>SCREEN_COLUMN-1 || y>SCREEN_ROW-1) {    //超出范围
        return 0;
    }
    return (screenBuffer[getBufferPart()][y/SCREEN_PAGE][x]>>(y%SCREEN_PAGE))&0x01;
}
/***    对临时缓冲进行一些操作    func:执行的功能可选择的参数如下
    BUFF_COPY,            将源缓冲区复制到目标缓冲区
    BUFF_CLEAN,            清除源缓冲区
    BUFF_COVER_L,        将源缓冲区取反再覆盖目标缓冲区
    BUFF_COVER_H        将源缓冲区覆盖目标缓冲区        ***/
void funcBuffer(int32_t func, int8_t partto, int8_t partfrom) {
    int32_t i = 0, j = 0;
    switch(func) {
    case BUFF_COPY:
        memcpy(screenBuffer[partto],screenBuffer[partfrom],sizeof(screenBuffer[partfrom]));
        break;
    case BUFF_CLEAN:
        clearBufferPart(partto, 0);    
        break;
    case BUFF_COVER_L:
        for(i=0; i<8; i++) {
            for(j=0; j<128; j++) {
                screenBuffer[partto][i][j] |= screenBuffer[partfrom][i][j];
            }
        }
        break;
    case BUFF_COVER_H:
        for(i=0;i<8;i++) {
            for(j=0; j<128; j++) {
                screenBuffer[partto][i][j] &= ~screenBuffer[partfrom][i][j];
            }
        }
        break;
    default:
        break;
    }
}
//将screenBuffer屏幕缓存的内容显示到屏幕上
void updateBuffer(void) {
    OLED_Fill(&screenBuffer[getBufferPart()]);
}
