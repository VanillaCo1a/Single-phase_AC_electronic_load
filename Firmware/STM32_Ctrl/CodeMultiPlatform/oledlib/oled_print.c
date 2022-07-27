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
//显示一个字符
//关于字体尺寸及使用请看SetFontSize()的注释
//当size=0时, y为第几行,x为第几列
void OLED_Char(int32_t y, int32_t x, uint8_t c) {
    int32_t i, j;
    uint8_t draw_background,bg,a,b,size,color;
    
    size = GetFontSize();        //字体尺寸
    color = getLineColor();    //字体颜色 1白0黑
    bg = GetTextBkMode();        //写字的时候字的背景的颜色 1白0黑
    draw_background = bg != color;    //这两个颜色要不一样字才看得到
    
    if(!size) {        //默认字符大小
        if((y>6) || (x>SCREEN_COLUMN-8)) {
            return;
        }
        c = c - ' ';            //得到偏移后的位置
        for(i=0; i<8; i++) {
            writeByteBuffer(y,x+i,Font_8x16[c*16+i]);
        }
        for(i=0; i<8; i++) {
            writeByteBuffer(y+1,x+i,Font_8x16[c*16+i+8]);
        }
    }else {                //使用原作粗体字符    
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
}
//显示字符串 就是显示多次显示字符
void OLED_String(int32_t y, int32_t x, char *str) {
    uint8_t j =  0, tempy = y, tempx = x;
    uint8_t size = GetFontSize();
    
    if(!size) {            //默认字体
        while (str[j]!='\0') {
            OLED_Char(y,x,str[j]);
            x += 8;
            if(x > 120) {
                x = 0;
                y += 2;
            }
            j++;
        }
    }else {                    //使用原作粗体字符
        while (str[j] != '\0') {
            if(str[j] == '\n') {
                tempx += 8*size;
                tempy = y;
                j++;
                continue;
            }
            OLED_Char(tempy,tempx,str[j]);
            tempy += size*6;
            j++;
        }
    }
}
//显示数字 就是多次显示数字的字符
void OLED_Num(uint8_t y, uint8_t x, uint32_t num, uint8_t len) {
    uint8_t t, temp;
    uint8_t enshow = 0;
    uint8_t size = GetFontSize();
    
    if(!size) {
        for(t=0; t<len; t++) {
            temp = (num/oled_pow(10,len-t-1))%10;
            if(enshow==0 && t<(len-1)) {
                if(temp == 0) {
                    OLED_Char(y,x+8*t,' ');
                    continue;
                }else {
                    enshow = 1;
                }
            }
            OLED_Char(y,x+8*t,temp+'0'); 
        }
    }else {
        for(t=0; t<len; t++) {
            temp = (num/oled_pow(10,len-t-1))%10;
            if(enshow==0 && t<(len-1)) {
                if(temp == 0) {
                    OLED_Char(y+(size*6)*t,x,'0');
                    continue;
                }else {
                    enshow = 1;
                }                    
            }
            OLED_Char(y+(size*6)*t,x,temp+'0'); 
        }
    }
}
void OLED_IntNum(uint8_t y, uint8_t x, int32_t num, uint8_t len) {
    int32_t temp;
    if(len > 0) {
        if(num > 0) {
            OLED_Char(y, x, '+');
            OLED_Num(y, x+8, num, len-1);
        }else {
            OLED_Char(y, x, '-');
            temp = abs(num);
            OLED_Num(y, x+8, temp, len-1);
        }
    }
}
//显示汉字
void OLED_Chinese(uint8_t y, uint8_t x, uint8_t *cn) {                      
    uint8_t j, wordNum;
    if((y > 7) || (x>128-16)) {
        return;
    }
    
    while(*cn != '\0') {                    //在C语言中字符串结束以‘\0’结尾
        for(wordNum=0; wordNum<117; wordNum++)    {
            if((CN16CHAR[wordNum].Index[0]==*cn) && (CN16CHAR[wordNum].Index[1]==*(cn+1)) && (CN16CHAR[wordNum].Index[0]==*(cn+2))) {    //查询要写的字在字库中的位置
                for(j=0; j<32; j++) {       //写一个字
                    if(j == 16) {           //由于16X16用到两个Y坐标，当大于等于16时，切换坐标
                        y++;
                    }            
                    writeByteBuffer(y,x+(j%16),CN16CHAR[wordNum].Msk[j]);
                }
                x += 16;
                y--;
                if(x > (128-16)) {
                    y += 2;
                    x = 0;
                }
                break;
            }
        }
        cn += 2;                            //此处打完一个字，接下来寻找第二个字
    }
}
//画一幅画
//起点坐标x y 图像取模数组 图像宽高w h
void OLED_Bitmap(int32_t y, int32_t x, uint8_t w, uint8_t h, const uint8_t *bitmap) {
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
//打印字符
void m_putchar(const char c) {
    uint8_t tmp = GetFontSize();
    if(c == '\n') {
        _cursor_y += 8;
        _cursor_x = 0;
    }else if(c != '\r') {
        SetFontSize(1);
        OLED_Char(_cursor_x, _cursor_y, c);
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
int32_t OLED_Printf(const char *str,...) {
    va_list ap;                                //定义一个可变 参数的（字符指针） 
    int32_t val,r_val;
    char count,ch;
    char *s = NULL;
    int32_t res = 0;                            //返回值

    va_start(ap,str);                    //初始化ap
    while('\0' != *str) {            //str为字符串,它的最后一个字符肯定是'\0'（字符串的结束符）
    
        switch(*str) {        //遇到百分号 此时要替换为参数
        case '%':
            str++;
            switch(*str) {    //10进制输出
            case 'd':
                val = va_arg(ap, int);
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
                val = va_arg(ap, int);
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
                s = va_arg(ap, char *);
                m_putstr(s);            //字符串,返回值为字符指针
                res += strlen(s);    //返回值长度增加?
                break;
            case 'c':
                m_putchar((char)va_arg(ap, int32_t ));    //大家猜为什么不写char，而要写int32_t 
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
    va_end(ap);
    return res;
}
