/***    原作者哔哩哔哩:                            MjGame         https://space.bilibili.com/38673747
            同GifHub:                                maoyongjie     https://github.com/hello-myj/stm32_oled/
            代码整理注释删减增加优化等 哔哩哔哩:    一只程序缘    https://space.bilibili.com/237304109
            整理之前的原代码随本代码一同提供,浏览以上网址获取更多相关信息,本代码以征得原作同意,感谢原作
    
        此c文件用于oled可直接调用的输出函数及printf函数
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
#define VA_BUF_SIZE 1024
static char va_buf[VA_BUF_SIZE] = {0};
void OLED_Printf(uint8_t y, uint8_t x, const char *str, ...) {
    uint8_t i, j;
    va_list args;
    char *pstr = va_buf;
    va_start(args, str);
    vsnprintf(va_buf, sizeof(va_buf), (char *)str, args);
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
