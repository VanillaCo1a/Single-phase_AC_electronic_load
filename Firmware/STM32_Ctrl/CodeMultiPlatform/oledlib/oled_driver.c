#include "oled_driver.h"

/***  OLED类, 初始化实例的样例如下:
#define SIZE_OLEDIO   sizeof(OLED_IOTypeDef) / sizeof(DEVIO_TypeDef)
#define SIZE_OLEDCMNI sizeof(OLED_CMNITypeDef) / sizeof(DEVCMNI_TypeDef)
void DEVIO_InitCallBack(void);
static OLED_PARTypeDef oled_parameter[] = {{.chip = OLED_SSD1306}};
static OLED_IOTypeDef oled_io[] = {
#if defined(STM32HAL)
    {{.SCL_SCK = {BOARD_OLED_SPI2_SCK_GPIO_Port, BOARD_OLED_SPI2_SCK_Pin},
      .SDA_SDO_TXD_OWRE = {BOARD_OLED_SPI2_SDO_GPIO_Port, BOARD_OLED_SPI2_SDO_Pin},
      .CS = {BOARD_OLED_CS_GPIO_Port, BOARD_OLED_CS_Pin}},
     .DC = {BOARD_OLED_DC_GPIO_Port, BOARD_OLED_DC_Pin},
     .RST = {NULL, 0x00}},
#elif defined(STM32FWLIBF1)
    {{.SCL_SCK = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_12},
      .SDA_SDO_TXD_OWRE = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_13}}},
#endif
};
static I2C_SoftHandleTypeDef ahi2c = {.clockstretch = true, .arbitration = true};
static SPI_SoftHandleTypeDef ahspi = {};
static I2C_ModuleHandleTypeDef oled_mi2c[] = {
    {.cmni = {.protocol = I2C, .ware = HARDWARE, .bus = &hi2c1},
     .addr = OLED_I2CADDR1, .skip = false, .speed = DEVI2C_HIGHSPEED, .errhand = DEVI2C_LEVER1},
    {.cmni = {.protocol = I2C, .ware = SOFTWARE, .bus = &ahi2c},
     .addr = OLED_I2CADDR1, .skip = false, .speed = DEVI2C_HIGHSPEED, .errhand = DEVI2C_LEVER1}};
static SPI_ModuleHandleTypeDef oled_mspi[] = {
    {.cmni = {.protocol = SPI, .ware = HARDWARE, .bus = &hspi2},
     .skip = false, .duplex = DEVSPI_HALF_DUPLEX},
    {.cmni = {.protocol = SPI, .ware = SOFTWARE, .bus = &ahspi},
     .skip = false, .duplex = DEVSPI_HALF_DUPLEX}};
static DEVS_TypeDef oleds = {.type = OLED};
static DEV_TypeDef oled[] = {
    {.parameter = &oled_parameter[0],
     .io = {.num = SIZE_OLEDIO, .confi = (DEVIO_TypeDef *)&oled_io[0], .init = DEVIO_InitCallBack},
     .cmni = {.num = SIZE_OLEDCMNI, .confi = (DEVCMNI_TypeDef *)&oled_mspi[0], .init = NULL}}};     ***/


DEVS_TypeDef *oleds = NULL;
DEV_TypeDef *oled = NULL;
poolsize oledSize = 0;

char *oled_va_buf = NULL;
size_t oled_bufSize = 0;

/* OLEDIO配置回调函数 */
void DEVIO_InitCallBack(void) {
    //可复用于I2C和SPI通信的OLED引脚定义(*为必须接IO由芯片控制): SCL_SCK*,SDA_SDO_TXD_OWRE*,CS,DC(*SPI),RST
    //I2C/SPI通信的基础通信引脚(在device.c中初始化): SCL*,SDA*/SCK*,SDI_RXD*,CS
    //OLED的其他控制引脚,非必须引脚可据需择芯片控制/电路控制('为不建议的接线方法): CS(I2C),DC(*SPI),RST
    //[I2C]CS:片选(接IO/接GND), DC:I2C地址选择(接IO/接VCCGND), RST:复位(接IO/接主控芯片RST/接VCC')
    //[SPI]DC*:命令/数据选择(接IO), RST:复位(接IO/接主控芯片RST/接VCC')
#if defined(STM32HAL)
    //不同设备实际使用的通信引脚不同, 未定义的引脚不会被初始化
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    if(DEV_GetActDevCmni()->protocol == I2C) {
        if(((DEVCMNIIO_TypeDef *)((OLED_IOTypeDef *)DEV_GetActDevIo()))->CS.GPIOx != NULL) {    //初始化CS
            ((DEVCMNIIO_TypeDef *)((OLED_IOTypeDef *)DEV_GetActDevIo()))->CS.Init.Structure = GPIO_InitStructure;
            ((DEVCMNIIO_TypeDef *)((OLED_IOTypeDef *)DEV_GetActDevIo()))->CS.Init.State = DEVIO_PIN_SET;
        }
        if(((OLED_IOTypeDef *)DEV_GetActDevIo())->DC.GPIOx != NULL) {
            if(((I2C_ModuleHandleTypeDef *)DEV_GetActDevCmni())->addr == OLED_I2CADDR1) {    //初始化DC
                ((OLED_IOTypeDef *)DEV_GetActDevIo())->DC.Init.Structure = GPIO_InitStructure;
                ((OLED_IOTypeDef *)DEV_GetActDevIo())->DC.Init.State = DEVIO_PIN_RESET;
            } else {
                ((OLED_IOTypeDef *)DEV_GetActDevIo())->DC.Init.Structure = GPIO_InitStructure;
                ((OLED_IOTypeDef *)DEV_GetActDevIo())->DC.Init.State = DEVIO_PIN_SET;
            }
        }
    } else if(DEV_GetActDevCmni()->protocol == SPI) {
        ((OLED_IOTypeDef *)DEV_GetActDevIo())->DC.Init.Structure = GPIO_InitStructure;
        ((OLED_IOTypeDef *)DEV_GetActDevIo())->DC.Init.State = DEVIO_PIN_SET;
    }
    if(((OLED_IOTypeDef *)DEV_GetActDevIo())->RST.GPIOx != NULL) {
        ((OLED_IOTypeDef *)DEV_GetActDevIo())->RST.Init.Structure = GPIO_InitStructure;
        ((OLED_IOTypeDef *)DEV_GetActDevIo())->RST.Init.State = DEVIO_PIN_RESET;
    }
#elif defined(STM32FWLIB)
#endif
}

/* OLED构造函数 */
void OLED_Init(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize uSize, char *buf, size_t bSize) {
    oleds = devs;
    oled = dev;
    oledSize = uSize;
    oled_va_buf = buf;
    oled_bufSize = bSize;
    //初始化OLED类设备, 将参数绑定到设备池中, 并初始化通信引脚
    DEV_Init(oleds, oled, oledSize);

    if(DEV_SetActStream(oleds, 0) == 1) { DEV_Error(1); }
    OLED_DevInit(((OLED_PARTypeDef *)DEV_GetActDev()->parameter)->flip);
}

/* TODO: OLED析构函数 */
void OLED_Deinit(DEVS_TypeDef *devs, DEV_TypeDef dev[], poolsize size) {}


/***  OLED I2C/SPI通信驱动  ***/
//[四脚OLED]只能进行I2C通信, 没有其他控制引脚
//[七脚OLED][I2C]CS:片选,默认置高,通信时置低, DC:I2C地址选择,在通信时根据当前OLEDI2C地址配置电位
//[七脚OLED][SPI]DC*:命令/数据选择,在通信时根据写往OLED内命令/数据寄存器配置电位

/* OLED连续写函数 */
static bool OLED_Write(uint8_t *pdata, uint16_t size, uint8_t address) {
    bool res = false;
    void *handle = DEV_GetActDevCmni();
    if(DEV_GetActDevCmni()->protocol == I2C) {
        if(((DEVCMNIIO_TypeDef *)((OLED_IOTypeDef *)DEV_GetActDevIo()))->CS.GPIOx != NULL) {
            DEVIO_ResetPin(&((DEVCMNIIO_TypeDef *)((OLED_IOTypeDef *)DEV_GetActDevIo()))->CS);
        }
        if(((OLED_IOTypeDef *)DEV_GetActDevIo())->DC.GPIOx != NULL) {
            if(((I2C_ModuleHandleTypeDef *)handle)->addr == OLED_I2CADDR1) {
                DEVIO_ResetPin(&((OLED_IOTypeDef *)DEV_GetActDevIo())->DC);
            } else if(((I2C_ModuleHandleTypeDef *)handle)->addr == OLED_I2CADDR2) {
                DEVIO_SetPin(&((OLED_IOTypeDef *)DEV_GetActDevIo())->DC);
            }
        }
        res = DEVCMNI_Write(pdata, size, address);
        if(((DEVCMNIIO_TypeDef *)((OLED_IOTypeDef *)DEV_GetActDevIo()))->CS.GPIOx != NULL) {
            DEVIO_SetPin(&((DEVCMNIIO_TypeDef *)((OLED_IOTypeDef *)DEV_GetActDevIo()))->CS);
        }
    } else if(DEV_GetActDevCmni()->protocol == SPI) {
        if(address == 0X00) {
            DEVIO_ResetPin(&((OLED_IOTypeDef *)DEV_GetActDevIo())->DC);
        } else if(address == 0X40) {
            DEVIO_SetPin(&((OLED_IOTypeDef *)DEV_GetActDevIo())->DC);
        }
        res = DEVCMNI_Write(pdata, size, 0);
        DEVIO_SetPin(&((OLED_IOTypeDef *)DEV_GetActDevIo())->DC);
    }
    return res;
}

/* OLED写字节函数 */
static void OLED_WriteByte(uint8_t data, uint8_t address) {
    OLED_Write(&data, 1, address);
}


/***  OLED的驱动函数, 将前面的配置IO口,通信等操作封装集成为OLED驱动, 供外部调用  ***/

/* 配置器件内部寄存器  initial settings configuration */
static void OLED_Configure(void) {
    //1. 基本命令
    OLED_WriteByte(0xAE, 0X00);    //设置显示关(默认)/开: 0xAE显示关闭(睡眠模式),0xAF显示正常开启  Set Display OFF(RESET)/ON: 0xAE Display OFF(sleep mode),0xAF Display ON in normal mode
    OLED_WriteByte(0xA4, 0X00);    //设置从内存(默认)/完全显示: 0xA4从内存中显示,0xA5完全显示  Entire Display OFF(RESET)/ON: 0xA4 Output follows RAM content,0xA5 Output ignores RAM content
    OLED_WriteByte(0xA6, 0X00);    //设置正常(默认)/反向显示: 0xA6内存中0关闭显示1开启显示,0xA7内存中1关闭显示0开启显示  Set Normal/Inverse Display: 0xA6 Normal display,0xA7 Inverse display
    OLED_WriteByte(0x81, 0X00);    //设置对比度(默认0x7F)  Set Contrast Control
    OLED_WriteByte(0xFF, 0X00);    //对比度范围为0~255(0x00~0xFF)
    //2. 滚动命令
    //...
    //3. 寻址设置命令
    OLED_WriteByte(0x20, 0X00);    //设置内存寻址模式(默认0x10) Set Memory Addressing Mode
    OLED_WriteByte(0x02, 0X00);    //0x00水平寻址模式,0x01垂直寻址模式,0x02页面寻址模式(默认),其他无效  0x00 Horizontal Addressing Mode,0x01 Vertical Addressing Mode,0x02 Page Addressing Mode,others Invalid
    OLED_WriteByte(0xB0, 0X00);    //设置页起始地址(0xB0~0xB7,仅页寻址模式有效)  Set Page Start Address for Page Addressing Mode
    if(((OLED_PARTypeDef *)DEV_GetActDev()->parameter)->chip == OLED_SSD1306) {
        OLED_WriteByte(0x00, 0X00);    //设置显示起始列地址低4位
    } else if(((OLED_PARTypeDef *)DEV_GetActDev()->parameter)->chip == OLED_SH1106) {
        OLED_WriteByte(0x02, 0X00);    //设置显示起始列地址低4位
    }
    OLED_WriteByte(0x10, 0X00);    //设置列起始地址高4位(默认0x10,0x10~0x1F,仅页寻址模式有效)  Set Higher Column Start Address for Page Addressing Mode
                                   //命令0x21,0x22仅水平寻址和垂直寻址模式有效
    //4. 硬件配置命令(面板分辨率和布局相关)
    OLED_WriteByte(0x40, 0X00);    //设置显示起始行地址(默认0x40,0x40~0x7F)  Set Display Start Line ADDRESS
    OLED_WriteByte(0xA8, 0X00);    //设置复用比(默认0x3F)  Set Multiplex Ratio
    OLED_WriteByte(0x3F, 0X00);    //复用比的范围为16MUX~64MUX,等于设置的数值+1(0x0F~0x3F)
    OLED_WriteByte(0xD3, 0X00);    //设置显示偏移  Set Display Offset
    OLED_WriteByte(0x00, 0X00);    //通过COM设置从0d到63d的垂直位移(默认0x00)
    OLED_WriteByte(0xDA, 0X00);    //设置COM引脚硬件配置  Set COM Pins Hardware Configuration
    OLED_WriteByte(0x12, 0X00);    //第5位0/1: 禁用(默认)/启用COM左右重新映射, 第4位0/1: 顺序COM引脚/可选COM引脚配置(默认)
    OLED_WriteByte(0xA1, 0X00);    //(画面水平翻转控制,0xA1正常0xA0翻转)   设置段重映射: 0xA0列地址从0映射至SEG0,0xA1列地址从127映射至SEG0  Set Segment Re-map
    OLED_WriteByte(0xC8, 0X00);    //(画面垂直翻转控制,0xC8正常0xC0翻转)   设置COM输出扫描方向: 0xC0普通模式,从COM0扫描至COM[N–1]; 0xC8重映射模式,从COM[N-1]扫描到COM0(其中N为复用比)  Set COM Output Scan Direction
    //5. 定时驱动方案设置命令
    OLED_WriteByte(0xD5, 0X00);    //设置振荡器频率(默认0x8)和显示时钟分频比(默认0x0,即分频比为1)  Set Oscillator Frequency & Display Clock Divide Ratio
    OLED_WriteByte(0xF0, 0X00);    //高4位: 设置晶振频率,FOSC频率随高4位数值增减而增减(0x0~0xF); 低4位: 设置DCLK的分频比,分频比=高4位数值+1设置Pre-charge间隔
    OLED_WriteByte(0xD9, 0X00);    //设置Pre-charge间隔(默认0x22)  Set Pre-charge Period
    OLED_WriteByte(0xF1, 0X00);    //高4位： 第2阶段间隔(0x1~0xF), 高4位： 第1阶段间隔时钟周期(0x1~0xF)
    OLED_WriteByte(0xDB, 0X00);    //Set VCOMH(默认0x20)
    OLED_WriteByte(0x30, 0X00);    //0x00 0.65xVcc, 0x20 0.77xVCC, 0x30 0.83xVCC
}

/* OLED初始化函数 */
void OLED_DevInit(uint8_t flip) {
    OLED_Off();
    OLED_Reset();
    OLED_Configure();
    OLED_Clear();
    OLED_Flip(flip, flip);
    OLED_SetCursor(0, 0);
    OLED_On();
}

/* OLED错误处理函数 */
void OLED_Error(void) {
    if(DEV_GetActDev()->error == 1) {
        DEV_GetActDev()->error++;
        DEV_SetActState(10000);
    } else if(DEV_GetActDev()->error == 2) {
        if(DEV_GetActState() == idle) {
            OLED_DevInit(((OLED_PARTypeDef *)DEV_GetActDev()->parameter)->flip);
            DEV_GetActDev()->error = 0;
        }
    }
}

/* OLED开启屏幕函数 */
void OLED_On(void) {               //OLED唤醒
    OLED_WriteByte(0X8D, 0X00);    //设置电荷泵
    OLED_WriteByte(0X14, 0X00);    //开启电荷泵
    OLED_WriteByte(0XAF, 0X00);    //开启OLED显示
}

/* OLED关闭屏幕函数 */
void OLED_Off(void) {              //OLED休眠
    OLED_WriteByte(0XAE, 0X00);    //关闭OLED显示
    OLED_WriteByte(0X8D, 0X00);    //设置电荷泵
    OLED_WriteByte(0X10, 0X00);    //关闭电荷泵
}

/* OLED复位函数 */
void OLED_Reset(void) {
    if((((OLED_IOTypeDef *)DEV_GetActDevIo()))->RST.GPIOx != 0x00) {
        DEVIO_ResetPin(&(((OLED_IOTypeDef *)DEV_GetActDevIo()))->RST);
    }
    DEV_SetActState(10000);
    if((((OLED_IOTypeDef *)DEV_GetActDevIo()))->RST.GPIOx != 0x00) {
        DEVIO_SetPin(&(((OLED_IOTypeDef *)DEV_GetActDevIo()))->RST);
    }
}

/* OLED设置屏幕翻转函数 */
void OLED_Flip(int8_t horizontal, int8_t vertical) {
    if(horizontal == 0) {
        OLED_WriteByte(0xA1, 0X00);
    } else {
        OLED_WriteByte(0xA0, 0X00);
    }
    if(vertical == 0) {
        OLED_WriteByte(0xC8, 0X00);
    } else {
        OLED_WriteByte(0xC0, 0X00);
    }
}

/* 清屏(只清空屏幕) */
void OLED_Clear(void) {
    int8_t buf = getBufferPart();
    setBufferPart(SCREEN_PART - 1);
    OLED_clearBuffer();
    OLED_updateScreen();
    setBufferPart(buf);
}

/* OLED设置器件内部光标函数, 格式为(页,列) */
void OLED_SetCursor(uint8_t page, uint8_t col) {
    OLED_PARTypeDef *oled_par = (OLED_PARTypeDef *)DEV_GetActDev()->parameter;
    /* 对于SH1106芯片的屏幕, 显存起始地址为0x02 */
    uint8_t col_temp = col + ((oled_par->chip == OLED_SH1106) ? 0x02 : 0x00);
    uint8_t cursor[3] = {
        0xB0 | (page),                      //设置显示起始页地址(0~7)
        0x00 | (col_temp & 0x0F),           //设置显示起始列地址低4位(0~F)
        0x10 | ((col_temp & 0xF0) >> 4),    //设置显示起始列地址高4位(0~7(8 ))
    };
    OLED_Write(cursor, 3, 0X00);
}

/* 使用I2C/SPI从缓存区读取对应位置数据, 用连续页写入的方式刷新整个屏幕 */
void OLED_Fill(uint8_t (*Buffer)[SCREEN_PAGE][SCREEN_COLUMN]) {
    for(int8_t i = 0; i < sizeof(*Buffer) / sizeof((*Buffer)[0]); i++) {
        OLED_SetCursor(i, 0);
        OLED_Write((*Buffer)[i], sizeof((*Buffer)[0]) / sizeof((*Buffer)[0][0]), 0X40);
    }
}

/* 在指定坐标处直接刷新一个字节(页中的某一列,上低下高), 由于要重设光标,较为缓慢 */
void OLED_FillByte(uint8_t page, uint8_t col, uint8_t data) {
    OLED_SetCursor(page, col);
    OLED_WriteByte(data, 0X40);
}
