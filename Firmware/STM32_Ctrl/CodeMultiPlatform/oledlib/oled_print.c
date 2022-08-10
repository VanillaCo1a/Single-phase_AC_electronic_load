/***    原作者哔哩哔哩:                       MjGame         https://space.bilibili.com/38673747
        同GifHub:                            maoyongjie     https://github.com/hello-myj/stm32_oled/
        代码整理注释删减增加优化等 哔哩哔哩:    一只程序缘      https://space.bilibili.com/237304109
        整理之前的原代码随本代码一同提供,浏览以上网址获取更多相关信息,本代码以征得原作同意,感谢原作  ***/

/***    此部分用于配置oled
        注意!!! 
        如需演示动画效果 OledTimeMsFunc()需放入1ms中断等中1ms调用一次为库提供时间基准 原作是放入滴答定时器中

        对颜色进行操作
        在这里主要是画线线条(一般为白)和填充(一般为白)的颜色
        (在oled.font中还有一个字体背景的颜色(一般为黑))
        在划线画图和打点时都会访问此文件函数获取打点的颜色                
    
        对字体进行操作
        设置字体背景颜色(字体颜色同划线线条颜色getLineColor())
        字体尺寸  ***/

#include "oled_print.h"

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



/***    此部分用于画图和字符操作(高级操作)
        (主要)由打点为基础
        再 线 折线
        再画和填充 矩形 三角形 圆 椭圆 圆角矩形
        然后是 图片 字符 字符串
        最后是 汉字                        ***/

//画图光标
TypeXY _point = {0, 0};

////////////////////////////////////////////////////////////////////////////
//以下4个函数是对当前光标的设置 供以下绘制函数调用 用户不直接使用
int GetX(void) {
    return _point.x;
}
int GetY(void) {
    return _point.y;
}
TypeXY GetXY(void) {
    return _point;
}
void MoveTo(int x,int y) {
    _point.x = x;
    _point.y = y;
}
void LineTo(int x,int y) {
    TypeXY temp;
    temp.x = x;
    temp.y = y;
    DrawLine(_point.x,_point.y, temp.x,temp.y);
    _point = temp;

}
////////////////////////////////////////////////////////////////////////////
//绘制一个点
void DrawPixel(int x,int y) {
    setPointBuffer(x,y,getLineColor());
}
//画线 参数:起点坐标,终点坐标
void DrawLine(int x1,int y1,int x2,int y2) {
    unsigned short us;
    unsigned short usX_Current, usY_Current;

    int lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance;
    int lIncrease_X, lIncrease_Y;

    lDelta_X = x2 - x1; //计算坐标增量
    lDelta_Y = y2 - y1;

    usX_Current = x1;
    usY_Current = y1;

    if(lDelta_X > 0) {
        lIncrease_X = 1;             //设置单步正方向
    }else if(lDelta_X == 0) {
        lIncrease_X = 0;            //垂直线
    }else {
        lIncrease_X = -1;            //设置单步反方向
        lDelta_X = -lDelta_X;
    }
    //Y轴的处理方式与上图X轴的处理方式同理
    if(lDelta_Y > 0) {
        lIncrease_Y = 1;
    }else if(lDelta_Y == 0) {
        lIncrease_Y = 0;            //水平线 
    }else {
        lIncrease_Y = -1;
        lDelta_Y = -lDelta_Y;
    }
    //选取不那么陡的方向依次画点
    if( lDelta_X > lDelta_Y ) {
        lDistance = lDelta_X;
    }else {
        lDistance = lDelta_Y;
    }
    //依次画点 进入缓存区 画好后再刷新缓冲区就好啦
    for(us=0; us<=lDistance+1; us++) {            //画线输出
        setPointBuffer(usX_Current,usY_Current,getLineColor());    //画点
        lError_X += lDelta_X ;
        lError_Y += lDelta_Y ;

        if(lError_X > lDistance) {
            lError_X -= lDistance;
            usX_Current += lIncrease_X;
        }
        if(lError_Y > lDistance) {
            lError_Y -= lDistance;
            usY_Current += lIncrease_Y;
        }
    }
}
//快速画线 专用于画横平竖直的线 提高效率
void DrawFastHLine(int x, int y, unsigned char w) {
    int end = x + w;
    int a;
    
    Type_color color = getLineColor();
    for(a=MAX(0,x); a<MIN(end,SCREEN_COLUMN); a++) {
        setPointBuffer(a,y,color);
    }
}
//快速画线 专用于画横平竖直的线 提高效率
void DrawFastVLine(int x, int y, unsigned char h) {
    int end = y + h;
    int a;
    
    Type_color color = getLineColor();
    for(a=MAX(0,y); a<MIN(end,SCREEN_ROW); a++) {
        setPointBuffer(x,a,color);
    }
}
//绘制折线 开始和转折点的坐标 总点个数
void DrawPolyLineTo(const TypeXY *points,int num) {
    int i = 0;
    MoveTo(points[0].x,points[0].y);
    for(i=1; i<num; i++) {
        LineTo(points[i].x,points[i].y);
    }
}

////////////////////////////////////////////////////////////////////////////
//使用对角点填充矩形
void DrawRect1(int left,int top,int right,int bottom) {
    DrawLine(left,top,right,top);
    DrawLine(left,bottom,right,bottom);
    DrawLine(left,top,left,bottom);
    DrawLine(right,top,right,bottom);    
}
//功能:使用对角点填充矩形
void DrawFillRect1(int left,int top,int right,int bottom) {
    DrawRect1(left,top,right,bottom);
    FillRect(left+1,top+1,right-left-1,bottom-top-1);
}
//左上角坐标 矩形宽高
void DrawRect2(int left,int top,int width,int height) {
    DrawLine(left,top,left+width-1,top);
    DrawLine(left,top+height-1,left+width-1,top+height-1);
    DrawLine(left,top,left,top+height-1);
    DrawLine(left+width-1,top,left+width-1,top+height-1);    
}
//填充矩形
void DrawFillRect2(int left,int top,int width,int height) {
    //先用上面的函数画外框
    DrawRect2(left,top,width,height);
    //然后填充实心
    FillRect(left+1,top+1,width-1,height-1);
}

////////////////////////////////////////////////////////////////////////////
//画圆
void DrawCircle (int usX_Center,int usY_Center,int usRadius) {
    short sCurrentX, sCurrentY;
    short sError;
    sCurrentX = 0;
    sCurrentY = usRadius;      
    sError = 3 - ( usRadius << 1 );     //判断下个点位置的标志
    
    while(sCurrentX <= sCurrentY) {
        //此处画圆打点的方法和画圆角矩形的四分之一圆弧的函数有点像
        setPointBuffer(usX_Center+sCurrentX,usY_Center+sCurrentY,getLineColor());        //1，研究对象
        setPointBuffer(usX_Center-sCurrentX,usY_Center+sCurrentY,getLineColor());        //2
        setPointBuffer(usX_Center-sCurrentY,usY_Center+sCurrentX,getLineColor());        //3
        setPointBuffer(usX_Center-sCurrentY,usY_Center-sCurrentX,getLineColor());        //4
        setPointBuffer(usX_Center-sCurrentX,usY_Center-sCurrentY,getLineColor());        //5
        setPointBuffer(usX_Center+sCurrentX,usY_Center-sCurrentY,getLineColor());        //6
        setPointBuffer(usX_Center+sCurrentY,usY_Center-sCurrentX,getLineColor());        //7
        setPointBuffer(usX_Center+sCurrentY,usY_Center+sCurrentX,getLineColor());        //0
        sCurrentX++;
        if(sError < 0) {
            sError += 4*sCurrentX + 6;
        }else {
            sError += 10 + 4*(sCurrentX-sCurrentY);
            sCurrentY--;
        }
    }
}
//填充圆
void DrawFillCircle(int usX_Center,int usY_Center,int r) {
    DrawFastVLine(usX_Center, usY_Center-r, 2*r+1);
    DrawFillCircleHelper(usX_Center,usY_Center, r, 3, 0);
}

////////////////////////////////////////////////////////////////////////////
//画部分圆
//圆心坐标 半径 4份圆要画哪一份或哪几份
void DrawCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername) {
    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;
    
    Type_color color = getLineColor();
    while(x < y) {
        if(f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        if(cornername & 0x4) {        //右上
            //此处画圆的方式是交替打点 从2边打到中间 最终x<y就打完点跳出循环
            setPointBuffer(x0+x,y0+y,color);
            setPointBuffer(x0+y,y0+x,color);
        }
        if(cornername & 0x2) {        //右下
            setPointBuffer(x0 + x, y0 - y, color);
            setPointBuffer(x0 + y, y0 - x, color);
        }
        if(cornername & 0x8) {        //左上
            setPointBuffer(x0-y,y0+x,color);
            setPointBuffer(x0-x,y0+y,color);
        }
        if(cornername & 0x1) {        //左下
            setPointBuffer(x0-y,y0-x,color);
            setPointBuffer(x0-x,y0-y,color);
        }
    }
}
//填充2个四分之一圆和中间的矩形
//此函数专用于画圆角矩形
//右上四分之一圆或左下四分之一圆的圆心 半径 中间矩形的高
void DrawFillCircleHelper(int x0, int y0, unsigned char r, unsigned char cornername, int delta) {
    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;
    
    //Type_color color = getLineColor();
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {        //填充右边的2个四分之一圆和中间的矩形
            DrawFastVLine(x0+x, y0-y, 2*y+1+delta);
            DrawFastVLine(x0+y, y0-x, 2*x+1+delta);
        }
        if (cornername & 0x2) {        //填充左边的2个四分之一圆和中间的矩形
            DrawFastVLine(x0-x, y0-y, 2*y+1+delta);
            DrawFastVLine(x0-y, y0-x, 2*x+1+delta);
        }
    }
}
//绘制一个圆弧
//x,y:圆弧中心坐标
//r:圆弧的半径
//angle_start:圆弧起始角度
//angle_end:圆弧终止角度
//注意：慎用此方法，此方法还需优化。
void DrawArc(int x,int y,unsigned char r,int angle_start,int angle_end) {
    float i = 0;
    TypeXY m, temp;
    temp = GetXY();
    SetRotateCenter(x,y);
    SetAnggleDir(0);
    if(angle_end > 360) {
        angle_end = 360;
    }
    SetAngle(0);
    m = GetRotateXY(x,y+r);
    MoveTo(m.x,m.y);
    for(i=angle_start; i<angle_end; i+=5) {
        SetAngle(i);
        m = GetRotateXY(x,y+r);
        LineTo(m.x,m.y);
    }
    LineTo(x+r,y);
    MoveTo(temp.x,temp.y);
}
void DrawFillArc(int x,int y,unsigned char r,int angle_start,int angle_end) {
    return;
}

////////////////////////////////////////////////////////////////////////////
//画圆角矩形
void DrawRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r) {
    //画出边缘 因为边缘是直线 所以专用高效率函数
    DrawFastHLine(x+r, y, w-2*r);         // Top
    DrawFastHLine(x+r, y+h-1, w-2*r);     // Bottom
    DrawFastVLine(x, y+r, h-2*r);         // Left
    DrawFastVLine(x+w-1, y+r, h-2*r);     // Right
    //画出四个圆角
    DrawCircleHelper(x+r, y+r, r, 1);
    DrawCircleHelper(x+w-r-1, y+r, r, 2);
    DrawCircleHelper(x+w-r-1, y+h-r-1, r, 4);
    DrawCircleHelper(x+r, y+h-r-1, r, 8);
}
//画实心圆角矩形
void DrawfillRoundRect(int x, int y, unsigned char w, unsigned char h, unsigned char r) {
  //画实心矩形
  DrawFillRect2(x+r, y, w-2*r, h);

  //再填充左右两边
  DrawFillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1);    //右上角的圆心
  DrawFillCircleHelper(x+r, y+r, r, 2, h-2*r-1);        //左下角的圆心
}

////////////////////////////////////////////////////////////////////////////
//画椭圆
//圆心 2个轴长
void DrawEllipse(int xCenter,int yCenter,int Rx,int Ry) {
    int Rx2 = Rx * Rx;
    int Ry2 = Ry * Ry;
    int twoRx2 = 2 * Rx2;
    int twoRy2 = 2 * Ry2;
    int p;
    int x = 0;
    int y = Ry;
    int px = 0;
    int py = twoRx2 * y;
    
    //椭圆最下面的点
    setPointBuffer(xCenter+x,yCenter+y,getLineColor());    //因为此时x=0 俩个点为同一个 原作这样写的 那就这样吧
    setPointBuffer(xCenter-x,yCenter+y,getLineColor());
    //椭圆最上面的点
    setPointBuffer(xCenter+x,yCenter-y,getLineColor());
    setPointBuffer(xCenter-x,yCenter-y,getLineColor());
    
    //Region?1 画出上下的线条 说实话我也没看懂了 算法大佬
    p = (int)(Ry2-Rx2*Ry + 0.25*Rx2);
    while(px < py) {
        x++;
        px += twoRy2;
        if(p < 0) {
            p+=Ry2+px;
        }else {
            y--;
            py -= twoRx2;
            p += Ry2 + px-py;
        }
        setPointBuffer(xCenter+x,yCenter+y,getLineColor());
        setPointBuffer(xCenter-x,yCenter+y,getLineColor());
        setPointBuffer(xCenter+x,yCenter-y,getLineColor());
        setPointBuffer(xCenter-x,yCenter-y,getLineColor());
    }
    
    //Region?2
    p = (int)(Ry2*(x+0.5)*(x+0.5) + Rx2*(y-1)*(y-1)-Rx2*Ry2);
    while(y >0 ) {
        y--;
        py -= twoRx2;
        if(p > 0) {
            p += Rx2 - py;
        }else {
            x++;
            px += twoRy2;
            p += Rx2 - py + px;
        }
        setPointBuffer(xCenter+x,yCenter+y,getLineColor());
        setPointBuffer(xCenter-x,yCenter+y,getLineColor());
        setPointBuffer(xCenter+x,yCenter-y,getLineColor());
        setPointBuffer(xCenter-x,yCenter-y,getLineColor());
    }
}
//填充一个椭圆 参数同上
void DrawFillEllipse(int x0, int y0,int rx,int ry) {
    int x, y;
    int xchg, ychg;
    int err;
    int rxrx2;
    int ryry2;
    int stopx, stopy;

    rxrx2 = rx;
    rxrx2 *= rx;
    rxrx2 *= 2;

    ryry2 = ry;
    ryry2 *= ry;
    ryry2 *= 2;

    x = rx;
    y = 0;

    xchg = 1;
    xchg -= rx;
    xchg -= rx;
    xchg *= ry;
    xchg *= ry;

    ychg = rx;
    ychg *= rx;

    err = 0;

    stopx = ryry2;
    stopx *= rx;
    stopy = 0;

    while(stopx >= stopy) {
        DrawFastVLine( x0+x, y0-y, y+1);
        DrawFastVLine( x0-x, y0-y, y+1);
        DrawFastVLine( x0+x, y0, y+1);
        DrawFastVLine( x0-x, y0, y+1);
        y++;
        stopy += rxrx2;
        err += ychg;
        ychg += rxrx2;
        if(2*err+xchg > 0) {
            x--;
            stopx -= ryry2;
            err += xchg;
            xchg += ryry2;      
        }
    }

    x = 0;
    y = ry;

    xchg = ry;
    xchg *= ry;

    ychg = 1;
    ychg -= ry;
    ychg -= ry;
    ychg *= rx;
    ychg *= rx;

    err = 0;
    stopx = 0;
    stopy = rxrx2;
    stopy *= ry;

    while(stopx <= stopy) {
        DrawFastVLine( x0+x, y0-y, y+1);
        DrawFastVLine( x0-x, y0-y, y+1);
        DrawFastVLine( x0+x, y0, y+1);
        DrawFastVLine( x0-x, y0, y+1);

        x++;
        stopx += ryry2;
        err += xchg;
        xchg += ryry2;
        if(2*err+ychg > 0) {
            y--;
            stopy -= rxrx2;
            err += ychg;
            ychg += rxrx2;
        }
    }
}
//功能:绘制一个矩形内切椭圆
//x0,y0:矩形左上角坐标
//x1,y1:矩形右下角坐标
void DrawEllipseRect( int x0, int y0, int x1, int y1) {
    int a = abs(x1 - x0);
    int b = abs(y1 - y0);    //get diameters
    int b1 = b & 1;
    long dx = 4 * (1-a) * b*b;
    long dy = 4 * (b1+1) * a*a;
    long err = dx + dy + b1*a*a;
    long e2;
    
    if(x0 > x1) {
        x0 = x1;
        x1 += a;
    }
    if(y0 > y1) {
        y0 = y1;
    } 
    y0 += (b+1) / 2;
    y1 = y0 - b1;
    a *= 8*a;
    b1 = 8*b*b;
    
    do {
        DrawPixel(x1, y0);
        DrawPixel(x0, y0);
        DrawPixel(x0, y1);
        DrawPixel(x1, y1);
        e2 = 2 * err;
        if(e2 >= dx) {
            x0++;
            x1--;
            err += dx += b1;
        }
        if(e2 <= dy) {
            y0++;
            y1--;
            err += dy += a;
        }
    }while(x0 <= x1);
    
    while(y0-y1 < b) {
        DrawPixel( x0-1, y0);
        DrawPixel( x1+1, y0++);
        DrawPixel( x0-1, y1);
        DrawPixel( x1+1, y1--);
    }
}

////////////////////////////////////////////////////////////////////////////
//画三角形
void DrawTriangle(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2) {
    //很简单  就是画3条任意线
    DrawLine(x0, y0, x1, y1);
    DrawLine(x1, y1, x2, y2);
    DrawLine(x2, y2, x0, y0);
}
//填充三角形
void DrawFillTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
    int a, b, y, last;
    int dx01, dy01,dx02,dy02,dx12,dy12,sa = 0,sb = 0;
    
    Type_color tmpcolor;
    tmpcolor = getLineColor();
    setLineColor(getFillColor());
    if(y0 > y1) {
        SWAP(y0, y1);
        SWAP(x0, x1);
    }
    if(y1 > y2) {
        SWAP(y2, y1);
        SWAP(x2, x1);
    }
    if(y0 > y1) {
        SWAP(y0, y1);
        SWAP(x0, x1);
    }
    if(y0 == y2) {
        a = b = x0;
        if(x1 < a) {
            a = x1;
        }else if(x1 > b) {
            b = x1;
        }
        if(x2 < a) {
            a = x2;
        }
        else if(x2 > b) {
            b = x2;
        }
        DrawFastHLine(a, y0, b-a+1);
        return;
    }
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa = 0,
    sb = 0;
    if (y1 == y2) {
        last = y1;   // Include y1 scanline
    }else {
        last = y1 - 1; // Skip it
    }

    for(y=y0; y<=last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;

        if(a > b) {
            SWAP(a,b);
        }
        DrawFastHLine(a, y, b-a+1);
    }
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if(a > b) {
            SWAP(a,b);
        }
        DrawFastHLine(a, y, b-a+1);
    }
    setLineColor(tmpcolor);
}



/***    此部分用于oled可直接调用的输出函数及printf函数
        printf函数类似串口调试printf, 不同的是发送的数据被显示在液晶屏上        ***/

#include "oled_print.h"

uint32_t oled_pow(uint8_t m,uint8_t n) {
    uint32_t result = 1;
    while(n--)result *= m;
    return result;
}
uint8_t pgm_read_byte(const uint8_t *addr) {
    return *addr;
}

static void printChar(uint8_t y, uint8_t x, char ch) {
    //关于字体尺寸及使用请看SetFontSize()的注释
    //当size=0时, y为第几行,x为第几列
    uint8_t i, j, a, b;
    uint8_t draw_background, bg, color, size;
    size = GetFontSize();               //字体尺寸
    color = getLineColor();             //字体颜色 1白0黑
    bg = GetTextBkMode();               //写字的时候字的背景的颜色 1白0黑
    draw_background = bg != color;      //这两个颜色要不一样字才看得到

    if(!size) {                         //默认字符大小
        ch = ch - ' ';                  //得到偏移后的位置
        for(i=0; i<8; i++) {
            writeByteBuffer(y/8,x+i,Font_8x16[ch*16+i]);
        }
        for(i=0; i<8; i++) {
            writeByteBuffer(y/8+1,x+i,Font_8x16[ch*16+i+8]);
        }
    }else {                //使用原作粗体字符
        for(i=0; i<6; i++) {
            int32_t line;
            //一个字符在Font_5x7中由一行6个char表示
            //line为这个字符的第某行内容
            if(i == 5) {
                line = 0x0;
            }else {
                line = pgm_read_byte(&Font_5x7[ch*5+i]);
            }
            for(j=0; j<8; j++) {
                uint8_t draw_color = (line & 0x1) ? color : bg;//目前需要填充的颜色是0 就是背景色 1就是字体色
                //不同号大小的字体只是最基础字体的放大倍数 这点要注意
                //比如基础字是1个像素 放大后就是4个像素 再就是9个像素 达到马赛克的放大效果
                if(draw_color || draw_background) {
                    for(a=0; a<size; a++) {
                        for(b=0; b<size; b++) {
                            setPointBuffer(x+(i*size)+a, y+(j*size)+b, draw_color);
                        }
                    }
                    line >>= 1;
                }
            }
        }
    }
}
static void printChinese(uint8_t y, uint8_t x, char str[3]) {
    bool findcn = false;
    uint8_t i, j;
    for(i=0; i<sizeof(CN16CHAR)/sizeof(*CN16CHAR); i++) {
        if((CN16CHAR[i].Index[0]==str[0]) && 
        (CN16CHAR[i].Index[1]==str[1]) && 
        (CN16CHAR[i].Index[2]==str[2])) {      //查询要写的字在字库中的位置
            findcn = true;
            break;
        }
    }
    if(findcn) {
        for(j=0; j<32; j++) {                   //写一个字
            writeByteBuffer(y/8+(j/16),x+(j%16),CN16CHAR[i].Msk[j]);
        }
    } else {
        for(j=0; j<32; j++) {                   //写一个字
            writeByteBuffer(y/8+(j/16),x+(j%16),CN16CHAR[0].Msk[j]);
        }
    }
}
static void OLED_PrintCharin(uint8_t *y, uint8_t *x, char *str, bool iscn) {
    uint8_t lenx, leny, size;
    if((size=GetFontSize()) == 0) {     //默认字体
        lenx = 8;
        leny = 16;
    } else {                            //使用原作粗体字符
        lenx = size * 6;
        leny = size * 8;
    }
    if(iscn) {
        /* 中文字符的宽总为英文字符的2倍 */
        lenx *= 2;
        leny = leny;
    }

    do {
        /* Clip all around */
        if((*y+leny>SCREEN_ROW) || (*x+lenx>SCREEN_COLUMN) || 
            (*y<0) || (*x<0)) {                
            break;
        }

        if(iscn) {
            printChinese(*y, *x, str);
        } else {
            printChar(*y, *x, *str);
        }
    } while(0);

    /* 坐标移动 */
    *x += lenx;
    if(*x+lenx > SCREEN_COLUMN) {
        *x = 0;
        *y += leny;
        if(*y+leny > SCREEN_ROW) {
            *y = 0;
        }
    }
}

/* 显示一个字符 */
void OLED_PrintChar(uint8_t y, uint8_t x, char c) {
   OLED_PrintCharin(&y, &x, &c, 0);
}
/* 显示字符串, 就是显示多次显示字符 */
void OLED_PrintString(uint8_t y, uint8_t x, char *str) {
    while (*str!='\0') {
        OLED_PrintCharin(&y, &x, str, 0);
        str++;
    }
}
/* 显示汉字 */
void OLED_PrintChinese(uint8_t y, uint8_t x, char *cn) {       
    while(*cn != '\0') {                    //在C语言中字符串结束以‘\0’结尾
        OLED_PrintCharin(&y, &x, &cn[0], 1);
        cn += 3;                            //此处打完一个字，接下来寻找第二个字
    }
}
/* 显示数字, 就是多次显示数字的字符 */
void OLED_PrintNum(uint8_t y, uint8_t x, uint32_t num, uint8_t len) {
    bool enshow;
    uint8_t i, numbit;
    char ch;

    enshow = true;
    for(i=0; i<len; i++) {
        /* 从高位到低位计算数字的每一位值 */
        numbit = (num/oled_pow(10,len-i-1))%10;
        if(enshow && (numbit!=0 || i>=(len-1))) {
            enshow = false;
        }
        if(enshow) {
            ch = ' ';
        }else {
            ch = numbit+'0';
        }
        OLED_PrintCharin(&y, &x, &ch, 0);
    }
}
void OLED_PrintIntNum(uint8_t y, uint8_t x, int32_t num, uint8_t len) {
    char ch;
    if(len > 0) {
        if(num > 0) {
            ch = '+';
        }else {
            ch = '-';
            num = abs(num);
        }
        OLED_PrintCharin(&y, &x, &ch, 0);
    }
    OLED_PrintNum(y, x, num, len-1);
}
void OLED_Printf(uint8_t y, uint8_t x, const char *str, ...) {
    uint8_t i, j;
    va_list args;
    char *pstr = oled_va_buf;
    va_start(args, str);
    vsnprintf(oled_va_buf, oled_bufSize, (char *)str, args);
    va_end(args);

    //if(code == 0) {
        // todo: add gbk coding print
    //} else if(code == 1) {
        while (*pstr != '\0') {
            /* Count there are how many bit '1' before bit '0' */
            j = 0;
            for(i=0x80; i>0x00; i>>=1) {
                if(0x00 != (*pstr&i)) {
                    j++;
                } else {
                    break;
                }
            }
            if(j == 0) {
                /* Check is a num or a char */
                if(*pstr >= '0' && *pstr <= '9') {
                    OLED_PrintCharin(&y, &x, pstr, 0);
                } else {
                    OLED_PrintCharin(&y, &x, pstr, 0);
                }
                pstr++;
            } else if(j == 3) {
                OLED_PrintCharin(&y, &x, pstr, 1);
                pstr += 3;
            } else {
                /* Check whether there is bit '0' */
                if(j == 8) {
                    /* error byte, jump it */
                    pstr++;
                } else {
                    /* Unknown word, just jump it */
                    pstr += j;
                }
            }
        }
    //}
}

//画一幅画
//起点坐标x y 图像取模数组 图像宽高w h
void OLED_PrintBitmap(int32_t y, int32_t x, uint8_t w, uint8_t h, const uint8_t *bitmap) {
    int32_t iCol, a;
    int32_t yOffset = abs(y) % 8;
    int32_t sRow = y / 8;
    int32_t rows = h / 8;

    if(x+w<0 || x>SCREEN_COLUMN-1 || y+h<0 || y>SCREEN_ROW-1) {
        return;
    }
    if(y < 0) {
        sRow--;
        yOffset = 8 - yOffset;
    }

    if(h%8!=0) {
        rows++;
    }
    for(a=0; a<rows; a++) {
        int32_t bRow = sRow + a;
        if(bRow > (SCREEN_ROW/8)-1) {
            break;
        }
        if(bRow > -2) {
            for(iCol = 0; iCol<w; iCol++) {
                if(iCol+x > (SCREEN_COLUMN-1)) {
                    break;
                }
                if(iCol+x >= 0) {
                    if(bRow >= 0) {
                        if(getLineColor() == pix_white) {
                            uint8_t temp = readByteBuffer(bRow,x + iCol);
                            temp|=pgm_read_byte(bitmap+(a*w)+iCol) << yOffset;
                            writeByteBuffer(bRow,x + iCol,temp);
                        }else if(getLineColor() == pix_black) {
                            uint8_t temp = readByteBuffer(bRow,x + iCol);
                            temp&=~(pgm_read_byte(bitmap+(a*w)+iCol) << yOffset);
                            writeByteBuffer(bRow,x + iCol,temp);
                        }else {
                            uint8_t temp = readByteBuffer(bRow,x + iCol);
                            temp^=(pgm_read_byte(bitmap+(a*w)+iCol) << yOffset);
                            writeByteBuffer(bRow,x + iCol,temp);
                        }
                    }
                    if(yOffset && bRow<(SCREEN_ROW/8)-1 && bRow > -2) {
                        if(getLineColor() == pix_white) {
                            uint8_t temp = readByteBuffer(bRow+1,x + iCol);
                            temp|=pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset);
                            writeByteBuffer(bRow+1,x + iCol,temp);
                        }else if(getLineColor() == pix_black) {
                            uint8_t temp = readByteBuffer(bRow+1,x + iCol);
                            temp&=~(pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset));
                            writeByteBuffer(bRow+1,x + iCol,temp);
                        }else {
                            uint8_t temp = readByteBuffer(bRow+1,x + iCol);
                            temp^=pgm_read_byte(bitmap+(a*w)+iCol) >> (8-yOffset);
                            writeByteBuffer(bRow+1,x + iCol,temp);
                        }
                    }
                }
            }
        }
    }
}
//void OLED_Float(uint8_t Y,uint8_t X,double real,uint8_t N) 
//{
//   uint8_t   i_Count=1;
//   uint8_t   n[12]={0};
//   long   j=1;  
//   int32_t    real_int=0;
//   double decimal=0;
//   uint32_t   real_decimal=0;
//   if(real<0)
//     {
//         real_int=(int)(-real);
//     }
//     else
//     {
//         real_int=(int)real;
//   }
//     decimal=real-real_int;
//   real_decimal=decimal*1e4;
//   while(real_int/10/j!=0)
//   {
//      j=j*10;i_Count++;  
//   } 
//   n[0]=(real_int/10000)%10; 
//   n[1]=(real_int/1000)%10;
//   n[2]=(real_int/100)%10;
//   n[3]=(real_int/10)%10;
//   n[4]=(real_int/1)%10; 
//   n[5]='.';
//   n[6]=(real_decimal/1000)%10;
//   n[7]=(real_decimal/100)%10;
//   n[8]=(real_decimal/10)%10;
//   n[9]=real_decimal%10;
//   n[6+N]='\0'; 
//   for(j=0;j<10;j++) n[j]=n[j]+16+32;
//     if(real<0) 
//     {         
//         i_Count+=1;
//         n[5-i_Count]='-';
//     }
//   n[5]='.';
//   n[6+N]='\0';   
//   OLED_ShowString(X,Y,&n[5-i_Count],12); 
//}

// void OLED_Float2(uint8_t Y,uint8_t X,double real,uint8_t N1,uint8_t N2) 
//{
//   uint8_t   i_Count=1;
//   uint8_t   n[12]={0};
//   long   j=1;  
//   uint32_t   real_int=0;
//   double decimal=0;
//   uint32_t   real_decimal=0;
//   X=X*8;
//   real_int=(int)real;
//   //Dis_Num(2,0,real_int,5);
//   decimal=real-real_int;
//   real_decimal=decimal*1e4;
//   //Dis_Num(2,6,real_decimal,4);
//   while(real_int/10/j!=0)
//   {
//      j=j*10;i_Count++;  
//   } 
//   n[0]=(real_int/10000)%10; 
//   n[1]=(real_int/1000)%10;
//   n[2]=(real_int/100)%10;
//   n[3]=(real_int/10)%10;
// 
//   n[5]='.';
//   n[6]=(real_decimal/1000)%10;
//   n[7]=(real_decimal/100)%10;
//   n[8]=(real_decimal/10)%10;
//   n[9]=real_decimal%10;
//   n[6+N2]='\0'; 
//   for(j=0;j<10;j++) n[j]=n[j]+16+32;
//   n[5]='.';
//   n[6+N2]='\0';   
//   OLED_ShowString(X,Y,&n[5-N1],12); 
//}



////////////////////////////////////////////////////////////////////////////
static int32_t _cursor_x = 0;
static int32_t _cursor_y = 0;
static uint8_t wrap = 1;

void SetTextWrap(uint8_t w) {
    wrap = w;
}
void SetCursor(int32_t x, int32_t y) {
    _cursor_x = x;
    _cursor_y = y;
}
//次方函数
static unsigned long m_pow(int32_t x,int32_t y) {
    unsigned long sum = 1;
    
    while(y--) {
        sum *= x;
    }
    return sum;
}
static void OLED_SmallChar(int32_t y, int32_t x, uint8_t c) {
    int32_t i, j;
    uint8_t draw_background,bg,a,b,size,color;
    
    size = GetFontSize();        //字体尺寸
    color = getLineColor();    //字体颜色 1白0黑
    bg = GetTextBkMode();        //写字的时候字的背景的颜色 1白0黑
    draw_background = bg != color;    //这两个颜色要不一样字才看得到
    
    //判断一个字符的上下左右是否超出边界范围
    if((y >= SCREEN_COLUMN) ||                    // Clip right
        (x >= SCREEN_ROW) ||                    // Clip bottom
        ((y + 5 * size - 1) < 0) ||           // Clip left
        ((x + 8 * size - 1) < 0)) {            // Clip top
        return;
    }
    for(i=0; i<6; i++) {
        int32_t line;
        //一个字符在Font_5x7中由一行6个char表示
        //line为这个字符的第某行内容
        if(i == 5) {
            line = 0x0;
        }else {
            line = pgm_read_byte(Font_5x7+(c*5)+i);
        }
        for(j=0; j<8; j++) {
            uint8_t draw_color = (line & 0x1) ? color : bg;//目前需要填充的颜色是0 就是背景色 1就是字体色

            //不同号大小的字体只是最基础字体的放大倍数 这点要注意
            //比如基础字是1个像素 放大后就是4个像素 再就是9个像素 达到马赛克的放大效果
            if(draw_color || draw_background) {
                for(a=0; a<size; a++) {
                    for(b=0; b<size; b++) {
                        setPointBuffer(y+(i*size)+a, x+(j*size)+b, draw_color);
                    }
                }
                line >>= 1;
            }
        }
    }
}
//打印字符
void m_putchar(const char c) {
    uint8_t tmp = GetFontSize();
    if(c == '\n') {
        _cursor_y += 8;
        _cursor_x = 0;
    }else if(c != '\r') {
        SetFontSize(1);
        OLED_SmallChar(_cursor_x, _cursor_y, c);
        SetFontSize(tmp);
        _cursor_x += 6;
        if(wrap && (_cursor_x > (SCREEN_COLUMN - 6))) {
            m_putchar('\n');
        }
    }
}
//打印字符串
void m_putstr(const char *str) {
    while(*str) {
        m_putchar(*str++);
    }
}
char numtochar(int8_t num, _Bool larsma) {    //0小写, 1大写
    if(num < 10) {
        return num+'0';
    }else {
        if(larsma) {
            return num-10+'A';
        }else {
            return num-10+'0';
        }
    }
}
//模仿printf功能显示到oled上
int32_t OLED_PrintfDebug(const char *str, ...) {
    va_list args;                                //定义一个可变 参数的（字符指针） 
    int32_t val,r_val;
    char count,ch;
    char *s = NULL;
    int32_t res = 0;                            //返回值

    va_start(args,str);                    //初始化ap
    while('\0' != *str) {            //str为字符串,它的最后一个字符肯定是'\0'（字符串的结束符）
    
        switch(*str) {        //遇到百分号 此时要替换为参数
        case '%':
            str++;
            switch(*str) {    //10进制输出
            case 'd':
                val = va_arg(args, int);
                r_val = val;
                count = 0;

                while(r_val) {
                    count++;        //整数的长度
                    r_val /= 10;
                }
                res += count;    //返回值长度增加? 
                r_val = val;
                while(count) {
                    ch = r_val / m_pow(10,count - 1);
                    r_val %= m_pow(10,count - 1);
                    m_putchar(ch + '0');    //数字到字符的转换
                    count--;
                }
                break;
            case 'x':                //16进制输出 
                val = va_arg(args, int);
                r_val = val;
                count = 0;
                while(r_val) {
                    count++;        //整数的长度
                    r_val /= 16;
                }
                res += count;    //返回值长度增加?
                r_val = val;
                while(count) {
                    ch = r_val / m_pow(16, count - 1);
                    r_val %= m_pow(16, count - 1);
                    if(ch <= 9) {
                        m_putchar(ch + '0');    //数字到字符的转换
                    }else {
                        m_putchar(ch - 10 + 'a'); 
                        count--;
                    }
                }
                break;
            case's':                //发送字符串
                s = va_arg(args, char *);
                m_putstr(s);            //字符串,返回值为字符指针
                res += strlen(s);    //返回值长度增加?
                break;
            case 'c':
                m_putchar((char)va_arg(args, int32_t ));    //大家猜为什么不写char，而要写int32_t 
                res++;
                break;
            default :
                break;
            }
            break;
            
        //遇到字符串末尾 回车换行
        case '\n':
            m_putchar('\n');
            res++;
            break;
        
        case '\r':
            m_putchar('\r');
            res++;
            break;
        
        default :    //显示原来的第一个参数的字符串(不是..里的参数o)
            m_putchar(*str);
            res++;
            break;
        }
        str++;
    }
    
    //显示到底部了 从头刷新显示
    if(_cursor_y+8 >= 63) {
        _cursor_y = 0;
    }
    va_end(args);
    return res;
}
