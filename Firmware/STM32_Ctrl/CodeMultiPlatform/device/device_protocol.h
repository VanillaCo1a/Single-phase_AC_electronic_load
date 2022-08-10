#ifndef __DEVICE_PROTOCOL_H
#define __DEVICE_PROTOCOL_H
#include <stdint.h>
#include <stdbool.h>

#define TIMER_DELAY                       1     //延时自身耗时1us
#define DEVI2C_LOWSPEED_SCL_HIGH_TIME     3     //I2C低速模式
#define DEVI2C_LOWSPEED_SCL_LOW_TIME      97    //
#define DEVI2C_STANDARD_SCL_HIGH_TIME     3     //I2C标准模式
#define DEVI2C_STANDARD_SCL_LOW_TIME      7     //
#define DEVI2C_FASTMODE_SCL_HIGH_TIME     3     //I2C快速模式
#define DEVI2C_FASTMODE_SCL_LOW_TIME      1     //
#define DEVI2C_FASTMODEPLUS_SCL_HIGH_TIME 1     //I2C快速模式plus
#define DEVI2C_FASTMODEPLUS_SCL_LOW_TIME  1     //FMP以上需要保证SCL低电平期间SDA有足够的时间(或更大的电流)上拉, 实测1k上拉基本稳定
#define DEVI2C_HIGHSPEED_SCL_HIGH_TIME    0     //I2C高速模式
#define DEVI2C_HIGHSPEED_SCL_LOW_TIME     0     //仍需要0.5us左右的延时
#define DEVI2C_ULTRAFAST_SCL_HIGH_TIME    0     //I2C超快模式
#define DEVI2C_ULTRAFAST_SCL_LOW_TIME     0     //仍需要0.3us左右的延时

#define DEVI2C_EXCEPTION 0    //读取从机应答信号异常后的处理方式

#define DEVSPI_SCLK_HIGH_TIME 0
#define DEVSPI_SCLK_LOW_TIME  0

typedef enum {
    LOW,
    HIGH
} Potential_TypeDef;
typedef enum {
    IN,
    OUT
} Direct_TypeDef;
typedef enum {
#if defined(STM32)
#if defined(STM32HAL)
    DEVCMNI_OK = HAL_OK,
    DEVCMNI_ERROR = HAL_ERROR,
    DEVCMNI_BUSY = HAL_BUSY,
    DEVCMNI_TIMEOUT = HAL_TIMEOUT,
    DEVCMNI_UPDATE
#elif defined(STM32FWLIB)
    DEVCMNI_OK = 0,
    DEVCMNI_ERROR,
    DEVCMNI_BUSY,
    DEVCMNI_TIMEOUT,
    DEVCMNI_UPDATE
#else
    DEVCMNI_OK,
    DEVCMNI_ERROR,
    DEVCMNI_BUSY,
    DEVCMNI_TIMEOUT,
    DEVCMNI_UPDATE
#endif
#endif
} DEVCMNI_StatusTypeDef;

typedef enum {
    DEV_OK = 0,
    DEV_ERROR = -1,
    DEV_BUSY = 1,
    DEV_SET = 2
} DEV_StatusTypeDef;


/*****   MODULE STRUCTURE DEFINITION & FUNCTION DECLARAION OF I2C DEVICE COMMUNITCATION   *****/
typedef enum {
    DEVI2C_SCL,
    DEVI2C_SDA
} DEVI2C_WireTypeDef;
typedef enum {
    DEVI2C_LOWSPEED,        //MAX10kbps
    DEVI2C_STANDARD,        //MAX100kbps
    DEVI2C_FASTMODE,        //MAX400kbps
    DEVI2C_FASTMODEPLUS,    //MAX1Mbps
    DEVI2C_HIGHSPEED,       //MAX3.4Mbps, 由于电阻上拉需要约1us, IO浮空输出在这一频率下工作不稳定且速度存在瓶颈
    DEVI2C_ULTRAFAST,       //MAX5Mbps
} DEVI2C_SpeedTypeDef;
typedef enum {
    DEVI2C_NOERROR = 0,
    DEVI2C_TIMEOUT = 1,
    DEVI2C_ARBITRATION = 2,
    DEVI2C_NOACK = 3,
    DEVI2C_NOFOUND = 4,
    DEVI2C_NOANSWER = 5,
} DEVI2C_ErrorTypeDef;
typedef enum {
    DEVI2C_LEVER1 = 0,    //当从机未响应超时, 时钟拉伸超时, 发生总线仲裁时均进入错误处理函数
    DEVI2C_LEVER0 = 1,    //无需从机应答, 在时钟拉伸超时后继续读写下一位, 无视发生的总线仲裁
} DEVI2C_ErrhandTypeDef;
typedef struct {                      //I2C总线设备结构体
    uint8_t addr;                     //模块I2C地址
    bool skip;                        //是否跳过读/写内部寄存器
    DEVI2C_SpeedTypeDef speed;        //模块I2C速率
    DEVI2C_ErrhandTypeDef errhand;    //模块的错误处理方式
    void *bus;                        //I2C模拟/硬件总线句柄
} I2C_ModuleHandleTypeDef;


/*****   MODULE STRUCTURE DEFINITION & FUNCTION DECLARAION OF SPI DEVICE COMMUNITCATION   *****/
typedef enum {
    DEVSPI_FULL_DUPLEX,
    DEVSPI_HALF_DUPLEX
} DEVSPI_DuplexTypeTypeDef;
typedef struct {                        //SPI总线模块结构体
    bool skip;                          //是否跳过拉低片选
    DEVSPI_DuplexTypeTypeDef duplex;    //设备工作模式
    void *bus;                          //SPI模拟/硬件总线句柄
} SPI_ModuleHandleTypeDef;


/*****   MODULE STRUCTURE DEFINITION & FUNCTION DECLARAION OF UART DEVICE COMMUNITCATION   *****/
typedef struct {
    uint8_t *buf;
    volatile size_t size;
    volatile size_t count;
    volatile DEVCMNI_StatusTypeDef state;
} UART_BufferTypedef;
typedef struct {
    UART_BufferTypedef receive;
    UART_BufferTypedef transmit;
    bool usedma;
    bool checkidle;
    void *bus;
} UART_ModuleHandleTypeDef;


/*****   MODULE STRUCTURE DEFINITION & FUNCTION DECLARAION OF 1-WIRE DEVICE COMMUNITCATION   *****/
typedef struct {
    uint64_t rom;    //模块64位ROM编码
    bool skip;       //是否跳过ROM匹配
    void *bus;       //ONEWIRE总线句柄
} ONEWIRE_ModuleHandleTypeDef;


/*****   SOFTWARE FUNCTION DECLARAION OF I2C/SPI/1-WIRE DEVICE COMMUNITCATION   *****/
void DEVCMNI_SCL_Set(bool dir);
void DEVCMNI_SDA_OWRE_Set(bool dir);
void DEVCMNI_SCL_SCK_Out(bool pot);
void DEVCMNI_SDA_SDI_RXD_OWRE_Out(bool pot);
bool DEVCMNI_SCL_In(void);
bool DEVCMNI_SDA_OWRE_In(void);
bool DEVCMNI_SDO_In(void);
void DEVCMNI_CS_Out(bool pot);
void DEVCMNI_Error(int8_t err);
void DEVCMNI_Delayus(uint64_t us);
void DEVCMNI_Delayms(uint64_t ms);
int8_t DEVCMNI_Delayus_paral(uint64_t us);


/*****   SOFTWARE STRUCTURE DEFINITION & FUNCTION IMPLEMENTATION OF I2C DEVICE COMMUNITCATION   *****/
#ifdef DEVI2C_SOFTWARE_ENABLED
typedef struct {          //I2C模拟总线结构体
    bool clockstretch;    //是否启用时钟拉伸
    bool arbitration;     //是否启用总线仲裁
#ifdef DEVI2C_USEPOINTER
    void (*SCL_Set)(bool);
    void (*SDA_Set)(bool);
    void (*SCL_Out)(bool);
    void (*SDA_Out)(bool);
    bool (*SCL_In)(void);
    bool (*SDA_In)(void);
    void (*error)(int8_t err);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
    int8_t (*delayus_paral)(uint16_t us);
#endif    // DEVI2C_USEPOINTER
} I2C_SoftHandleTypeDef;
#if defined(DEVI2C_USEPOINTER)
#define DEVI2C_SCL_Set(dir)      i2cbus.SCL_Set(dir)
#define DEVI2C_SDA_Set(dir)      i2cbus.SDA_Set(dir)
#define DEVI2C_SCL_Out(pot)      i2cbus.SCL_Out(pot)
#define DEVI2C_SDA_Out(pot)      i2cbus.SDA_Out(pot)
#define DEVI2C_SCL_In()          i2cbus.SCL_In()
#define DEVI2C_SDA_In()          i2cbus.SDA_In()
#define DEVI2C_Error(err)        i2cbus.error(err)
#define DEVI2C_Delayus(us)       ({if(us) {i2cbus.delayus(us-TIMER_DELAY);} })
#define DEVI2C_Delayms(ms)       ({if(ms) {i2cbus.delayms(ms);} })
#define DEVI2C_Delayus_paral(us) i2cbus.delayus_paral(us)
#else
#define DEVI2C_SCL_Set(dir)      DEVCMNI_SCL_Set(dir)
#define DEVI2C_SDA_Set(dir)      DEVCMNI_SDA_OWRE_Set(dir)
#define DEVI2C_SCL_Out(pot)      DEVCMNI_SCL_SCK_Out(pot)
#define DEVI2C_SDA_Out(pot)      DEVCMNI_SDA_SDI_RXD_OWRE_Out(pot)
#define DEVI2C_SCL_In()          DEVCMNI_SCL_In()
#define DEVI2C_SDA_In()          DEVCMNI_SDA_OWRE_In()
#define DEVI2C_Error(err)        DEVCMNI_Error(err)
#define DEVI2C_Delayus(us)       ({if(us) {DEVCMNI_Delayus(us-TIMER_DELAY);} })
#define DEVI2C_Delayms(ms)       ({if(ms) {DEVCMNI_Delayms(ms);} })
#define DEVI2C_Delayus_paral(us) DEVCMNI_Delayus_paral(us)
#endif    // DEVI2C_USEPOINTER

static uint32_t DEVI2C_scl_lowtime = 0, DEVI2C_scl_hightime = 0, DEVI2C_timeout = 0;
static DEVI2C_ErrorTypeDef i2cerror = DEVI2C_NOERROR;
static I2C_SoftHandleTypeDef i2cbus = {.clockstretch = true, .arbitration = true};
static I2C_ModuleHandleTypeDef i2cmodular = {.addr = 0x00, .speed = DEVI2C_STANDARD, .errhand = DEVI2C_LEVER1, .bus = &i2cbus};
static int8_t DEVI2C_Init(I2C_ModuleHandleTypeDef *modular, uint32_t timeout) {
    i2cmodular = *modular;
    i2cbus = *((I2C_SoftHandleTypeDef *)modular->bus);
    i2cerror = DEVI2C_NOERROR;
    if(i2cmodular.speed >= DEVI2C_ULTRAFAST) {
        i2cmodular.errhand = DEVI2C_LEVER0;
        i2cbus.clockstretch = false;
        i2cbus.arbitration = false;
    }
    //todo: 根据不同平台设置延时时间/根据输入频率计算延迟时间
    if(modular->speed == DEVI2C_ULTRAFAST) {
        DEVI2C_scl_hightime = DEVI2C_ULTRAFAST_SCL_HIGH_TIME;
        DEVI2C_scl_lowtime = DEVI2C_ULTRAFAST_SCL_LOW_TIME;
    } else if(modular->speed == DEVI2C_HIGHSPEED) {
        DEVI2C_scl_hightime = DEVI2C_HIGHSPEED_SCL_HIGH_TIME;
        DEVI2C_scl_lowtime = DEVI2C_HIGHSPEED_SCL_LOW_TIME;
    } else if(modular->speed == DEVI2C_FASTMODEPLUS) {
        DEVI2C_scl_hightime = DEVI2C_FASTMODEPLUS_SCL_HIGH_TIME;
        DEVI2C_scl_lowtime = DEVI2C_FASTMODEPLUS_SCL_LOW_TIME;
    } else if(modular->speed == DEVI2C_FASTMODE) {
        DEVI2C_scl_hightime = DEVI2C_FASTMODE_SCL_HIGH_TIME;
        DEVI2C_scl_lowtime = DEVI2C_FASTMODE_SCL_LOW_TIME;
    } else if(modular->speed == DEVI2C_STANDARD) {
        DEVI2C_scl_hightime = DEVI2C_STANDARD_SCL_HIGH_TIME;
        DEVI2C_scl_lowtime = DEVI2C_STANDARD_SCL_LOW_TIME;
    } else if(modular->speed == DEVI2C_LOWSPEED) {
        DEVI2C_scl_hightime = DEVI2C_LOWSPEED_SCL_HIGH_TIME;
        DEVI2C_scl_lowtime = DEVI2C_LOWSPEED_SCL_LOW_TIME;
    }
    DEVI2C_timeout = timeout;

    return 0;
}
/* 以下的整个持续读写过程为原子操作, 每个函数(除Start())开始前与(除Stop())结束后总是有: SCL,SDA为IO输出模式, SCL为未释放状态(置低) */
/* 对于进一步切割, 使同一条总线上的读写操作可以并行运行的工作, 还有待探究 */
static inline bool DEVI2C_ClockStetch(void) {    //时钟拉伸判断函数
    if(i2cmodular.errhand == DEVI2C_LEVER0) {    //直接写下一位
        return 0;
    } else if(i2cmodular.errhand == DEVI2C_LEVER1) {    //等待时钟线释放直至超时
        DEVI2C_SCL_Set(IN);
        while(!DEVI2C_SCL_In()) {
            if(DEVI2C_Delayus_paral(DEVI2C_timeout)) {
                DEVI2C_SCL_Set(OUT);
                DEVI2C_SDA_Out(HIGH);    //退出进行错误处理前, 释放数据线
                i2cerror = DEVI2C_TIMEOUT;
                return 1;
            }
        }
        DEVI2C_SCL_Set(OUT);
    }
    return 0;
}
static inline bool DEVI2C_BusArbitration(void) {    //总线仲裁判断函数
    if(i2cmodular.errhand == DEVI2C_LEVER0) {       //直接写下一位
        return 0;
    } else if(i2cmodular.errhand == DEVI2C_LEVER1) {    //等待数据线释放直至超时
        DEVI2C_SDA_Set(IN);
        while(!DEVI2C_SDA_In()) {
            if(DEVI2C_Delayus_paral(DEVI2C_timeout)) {
                DEVI2C_SDA_Set(OUT);
                DEVI2C_SCL_Out(HIGH);    //退出进行错误处理前, 释放时钟线
                i2cerror = DEVI2C_ARBITRATION;
                return 1;
            }
        }
        DEVI2C_SDA_Set(OUT);
    }
    return 0;
}
static void DEVI2C_Start_(void);
static void DEVI2C_Stop_(void);
static bool DEVI2C_SlaveWaiting(void) {          //从机响应判断函数
    if(i2cmodular.errhand == DEVI2C_LEVER0) {    //直接写下一字节
        return 0;
    } else if(i2cmodular.errhand == DEVI2C_LEVER1) {    //等待从机响应直至超时
        while(DEVI2C_SDA_In()) {
            if(DEVI2C_Delayus_paral(DEVI2C_timeout)) {
                DEVI2C_SDA_Set(OUT);
                /* 可能导致从机未响应的原因:
                1.时钟太快, 从机跟不上未及时下拉数据线, 导致未响应
                2.时钟线被强上拉, 从机察觉不到时序, 间接导致未响应
                3.数据线被强上拉, 从机无法下拉数据线, 直接导致未响应
                尝试使用释放时钟线以结束至错误处理 */
                DEVI2C_SCL_Out(HIGH);    //退出进行错误处理前, 释放时钟线
                i2cerror = DEVI2C_NOACK;
                return 1;
            }
        }
    }
    return 0;
}
static void DEVI2C_Start(void) {
    /* 在时钟线置高时, 数据线上的一个下降沿表示开始传输数据 */
    DEVI2C_SCL_Out(LOW);
    DEVI2C_SDA_Out(HIGH);
    DEVI2C_Delayus(DEVI2C_scl_lowtime);        //时钟线置低期间等待数据线电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    if(DEVI2C_BusArbitration()) { return; }    //总线仲裁判断, 若发现数据线没有被如期置高, 则进行仲裁处理
    DEVI2C_SCL_Out(HIGH);                      //
    DEVI2C_Delayus(DEVI2C_scl_hightime);       //时钟线拉高后等待电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    if(DEVI2C_ClockStetch()) { return; }       //时钟拉伸判断, 若发现时钟线没有被如期置高, 则等待直至超时进行超时处理
    DEVI2C_SDA_Out(LOW);                       //拉低数据线, 产生一个下降沿
    DEVI2C_Delayus(DEVI2C_scl_hightime);       //时钟线置高期间等待数据线电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    DEVI2C_SCL_Out(LOW);                       //拉低时钟线
}
static void DEVI2C_Stop(void) {
    /* 在时钟线置高时, 数据线上的一个上升沿表示停止传输数据 */
    DEVI2C_SDA_Out(LOW);                       //拉低数据线, 进行准备
    DEVI2C_Delayus(DEVI2C_scl_lowtime);        //时钟线置低期间等待数据线电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    DEVI2C_SCL_Out(HIGH);                      //拉高时钟线
    DEVI2C_Delayus(DEVI2C_scl_hightime);       //时钟线拉高后等待电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    if(DEVI2C_ClockStetch()) { return; }       //时钟拉伸判断, 若发现时钟线没有被如期置高, 则等待直至超时进行超时处理
    DEVI2C_SDA_Out(HIGH);                      //拉高数据线, 产生一个上升沿
    DEVI2C_Delayus(DEVI2C_scl_hightime);       //时钟线置高期间等待数据线电位稳定
    __NOP();                                   //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    __NOP();                                   //
    if(DEVI2C_BusArbitration()) { return; }    //总线仲裁判断, 若发现数据线没有被如期置高, 则进行仲裁处理
}
static void DEVI2C_Start_(void) {
    /* 在时钟线置高时, 数据线上的一个下降沿表示开始传输数据 */
    DEVI2C_SCL_Out(LOW);
    DEVI2C_SDA_Out(HIGH);
    DEVI2C_Delayus(DEVI2C_scl_lowtime);     //时钟线置低期间等待数据线电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    DEVI2C_SCL_Out(HIGH);                   //
    DEVI2C_Delayus(DEVI2C_scl_hightime);    //时钟线拉高后等待电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    DEVI2C_SDA_Out(LOW);                    //拉低数据线, 产生一个下降沿
    DEVI2C_Delayus(DEVI2C_scl_hightime);    //时钟线置高期间等待数据线电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    DEVI2C_SCL_Out(LOW);                    //拉低时钟线
}
static void DEVI2C_Stop_(void) {
    /* 在时钟线置高时, 数据线上的一个上升沿表示停止传输数据 */
    DEVI2C_SDA_Out(LOW);                    //拉低数据线, 进行准备
    DEVI2C_Delayus(DEVI2C_scl_lowtime);     //时钟线置低期间等待数据线电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    DEVI2C_SCL_Out(HIGH);                   //拉高时钟线
    DEVI2C_Delayus(DEVI2C_scl_hightime);    //时钟线拉高后等待电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    DEVI2C_SDA_Out(HIGH);                   //拉高数据线, 产生一个上升沿
    DEVI2C_Delayus(DEVI2C_scl_hightime);    //时钟线置高期间等待数据线电位稳定
    __NOP();                                //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                //
    __NOP();                                //
    __NOP();                                //
    __NOP();
}
static inline void DEVI2C_WriteBit(bool bit) {    // 写1位数据
    /* I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 主机写数据; 时钟线拉高期间, 数据不可进行操作, 从机读数据 */
    DEVI2C_SDA_Out(bit);                              //写数据至数据线
    DEVI2C_Delayus(DEVI2C_scl_lowtime);               //时钟线置低期间等待数据线电位稳定
    __NOP();                                          //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                          //
    __NOP();                                          //
    __NOP();                                          //
    __NOP();                                          //
    if(bit && DEVI2C_BusArbitration()) { return; }    //总线仲裁判断, 若发现数据线没有被如期置高, 则进行仲裁处理
    DEVI2C_SCL_Out(HIGH);                             //拉高时钟线并保持一段时间, 等待从机读取
    DEVI2C_Delayus(DEVI2C_scl_hightime);              //时钟线拉高后等待电位稳定
    __NOP();                                          //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                          //
    __NOP();                                          //
    __NOP();                                          //
    __NOP();                                          //
    if(DEVI2C_ClockStetch()) { return; }              //时钟拉伸判断, 若发现时钟线没有被如期置高, 则等待直至超时进行超时处理
    DEVI2C_SCL_Out(LOW);                              //拉低时钟线
}
static inline void DEVI2C_WriteBit_(bool bit) {
    DEVI2C_SDA_Out(bit);
    DEVI2C_Delayus(DEVI2C_scl_lowtime);
    __NOP();    //tofix:将短暂的暂停优化到延时函数内
    __NOP();    //
    __NOP();    //
    __NOP();    //
    __NOP();    //
    DEVI2C_SCL_Out(HIGH);
    DEVI2C_Delayus(DEVI2C_scl_hightime);
    __NOP();    //tofix:将短暂的暂停优化到延时函数内
    __NOP();    //
    __NOP();    //
    __NOP();    //
    __NOP();    //
    DEVI2C_SCL_Out(LOW);
}
static inline bool DEVI2C_ReadBit(void) {    // 读1位数据
    /* I2C通信建立后, 在时钟线拉低期间, 数据线可进行操作, 从机写数据; 时钟线拉高期间, 数据不可进行操作, 主机读数据 */
    bool bit = 0;
    // DEVI2C_SDA_Out(HIGH);                     //释放数据总线
    // DEVI2C_SDA_Set(IN);                       //数据线设置为读取模式
    DEVI2C_Delayus(DEVI2C_scl_lowtime);          //时钟线置低期间等待数据线电位稳定
    __NOP();                                     //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                     //
    __NOP();                                     //
    __NOP();                                     //
    __NOP();                                     //
    DEVI2C_SCL_Out(HIGH);                        //拉高时钟线并保持一段时间, 待数据稳定后读入
    DEVI2C_Delayus(DEVI2C_scl_hightime);         //时钟线拉高后等待电位稳定
    __NOP();                                     //tofix:将短暂的暂停优化到延时函数内
    __NOP();                                     //
    __NOP();                                     //
    __NOP();                                     //
    __NOP();                                     //
    bit = DEVI2C_SDA_In();                       //自数据线读数据
    if(DEVI2C_ClockStetch()) { return true; }    //时钟拉伸判断, 若发现时钟线没有被如期置高, 则等待直至超时进行超时处理
    DEVI2C_SCL_Out(LOW);                         //拉低时钟线
    // DEVI2C_SDA_Set(OUT);                      //数据线设置为写入模式
    return bit;
}
static inline bool DEVI2C_ReadBit_(void) {
    bool bit = 0;
    // DEVI2C_SDA_Out(HIGH);
    // DEVI2C_SDA_Set(IN);
    DEVI2C_Delayus(DEVI2C_scl_lowtime);
    __NOP();    //tofix:将短暂的暂停优化到延时函数内
    __NOP();    //
    __NOP();    //
    __NOP();    //
    __NOP();    //
    DEVI2C_SCL_Out(HIGH);
    DEVI2C_Delayus(DEVI2C_scl_hightime);
    __NOP();    //tofix:将短暂的暂停优化到延时函数内
    __NOP();    //
    __NOP();    //
    __NOP();    //
    __NOP();    //
    bit = DEVI2C_SDA_In();
    DEVI2C_SCL_Out(LOW);
    // DEVI2C_SDA_Set(OUT);
    return bit;
}
static bool DEVI2C_Write(uint8_t *pdata, size_t size) {    // 连续写数据, 将数据按位拆分后写入
    /* 每写入1字节数据后等待从机应答, 写入完毕, 主机释放(即拉高)时钟线和数据线
     一定时间内若能检测到从机拉低数据线, 说明从机应答正常, 可以继续进行通信; 否则说明从机存在问题, 需要重新建立通信 */
    bool bit = 0;
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            DEVI2C_WriteBit(pdata[j] & i ? 1 : 0);
            if(i2cerror) { return 1; }    //i2cerror非0, 说明上层因时钟超时/总线仲裁而返回
        }
        DEVI2C_SDA_Out(HIGH);    //释放数据总线
        DEVI2C_SDA_Set(IN);      //数据线设置为读取模式
        if(DEVI2C_ReadBit()) {
            if((bit = i2cerror)) { break; }                 //i2cerror非0, 说明上层因时钟超时而返回
            if((bit = DEVI2C_SlaveWaiting())) { break; }    //从机响应判断, 若发现数据线没有被从机拉低, 则等待直至超时进行未响应处理
        }
        DEVI2C_SDA_Set(OUT);
    }
    DEVI2C_SDA_Set(OUT);
    return bit;
}
static bool DEVI2C_Write_(uint8_t *pdata, size_t size) {
    bool bit = 0;
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x80; i; i >>= 1) {
            DEVI2C_WriteBit_(pdata[j] & i ? 1 : 0);
        }
        DEVI2C_SDA_Out(HIGH);
        DEVI2C_SDA_Set(IN);
        if(DEVI2C_ReadBit()) {
            if((bit = DEVI2C_SlaveWaiting())) { break; }    //从机响应判断, 若发现数据线没有被从机拉低, 则等待直至超时进行未响应处理
        }
        DEVI2C_SDA_Set(OUT);
    }
    DEVI2C_SDA_Set(OUT);
    return bit;
}
static void DEVI2C_Read(uint8_t *pdata, size_t size) {    // 连续读数据, 将数据按位读取后合并
    /* 每读取1字节数据后主机进行应答, 读取完毕, 从机会释放(即拉高)数据线
    主机写入1位低电平信号, 表示应答正常, 从机继续进行通信; 主机写入1位高电平信号, 表示不再接收数据, 从机停止通信 */
    for(uint16_t j = 0; j < size; j++) {
        DEVI2C_SDA_Out(HIGH);    //释放数据总线
        DEVI2C_SDA_Set(IN);      //数据线设置为读取模式
        for(uint8_t i = 0x80; i; i >>= 1) {
            pdata[j] |= DEVI2C_ReadBit() ? i : 0x00;
            if(i2cerror) { return; }    //i2cerror非0, 说明上层因时钟超时而返回
        }
        DEVI2C_SDA_Set(OUT);    //数据线设置为写入模式
        if(j + 1 < size) {
            DEVI2C_WriteBit(1);
            if(i2cerror) { return; }    //i2cerror非0, 说明上层因时钟超时/总线仲裁而返回
        } else {
            DEVI2C_WriteBit(0);
        }
    }
}
static void DEVI2C_Read_(uint8_t *pdata, size_t size) {
    for(uint16_t j = 0; j < size; j++) {
        DEVI2C_SDA_Out(HIGH);
        DEVI2C_SDA_Set(IN);
        for(uint8_t i = 0x80; i; i >>= 1) {
            pdata[j] |= DEVI2C_ReadBit_() ? i : 0x00;
        }
        DEVI2C_SDA_Set(OUT);
        if(j + 1 < size) {
            DEVI2C_WriteBit_(1);
        } else {
            DEVI2C_WriteBit_(0);
        }
    }
}

__attribute__((unused)) static DEV_StatusTypeDef DEVI2C_Transmit(I2C_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, uint8_t address, bool rw, uint32_t timeout) {
    /* 多字节读写函数, timeout应答超时,speed速度模式,rw1为读0为写 */
    uint8_t byte;
    DEVI2C_Init(modular, timeout);
    if(i2cbus.clockstretch || i2cbus.arbitration) {
        do {
            DEVI2C_Start();
            if(i2cerror) { break; }
            if(modular->skip) {
                byte = (i2cmodular.addr << 1) | rw;
                if(DEVI2C_Write(&byte, 1)) {    //发送器件地址+读/写命令
                    if(i2cerror == DEVI2C_NOACK) { i2cerror = DEVI2C_NOFOUND; }
                    break;
                }
            } else {
                byte = (i2cmodular.addr << 1) | 0X00;
                if(DEVI2C_Write(&byte, 1)) {    //发送器件地址+写命令
                    if(i2cerror == DEVI2C_NOACK) { i2cerror = DEVI2C_NOFOUND; }
                    break;
                }
                if(DEVI2C_Write(&address, 1)) {    //发送寄存器地址
                    if(i2cerror == DEVI2C_NOACK) { i2cerror = DEVI2C_NOFOUND; }
                    break;
                }
                if(rw) {
                    DEVI2C_Start();
                    if(i2cerror) { break; }
                    byte = (i2cmodular.addr << 1) | 0X01;
                    if(DEVI2C_Write(&byte, 1)) {    //发送器件地址+读命令
                        if(i2cerror == DEVI2C_NOACK) { i2cerror = DEVI2C_NOFOUND; }
                        break;
                    }
                }
            }
            if(rw) {
                DEVI2C_Read(pdata, size);    //连续读取
                if(i2cerror) { break; }
            } else {
                if(DEVI2C_Write(pdata, size)) {    //连续写入
                    if(i2cerror == DEVI2C_NOACK) { i2cerror = DEVI2C_NOANSWER; }
                    break;
                }
            }
            DEVI2C_Stop();
            if(i2cerror) { break; }
            return DEV_OK;
        } while(0);
        DEVI2C_Error(i2cerror);
        return DEV_ERROR;
    } else {
        DEVI2C_Start_();
        if(modular->skip) {
            byte = (i2cmodular.addr << 1) | rw;
            if(DEVI2C_Write_(&byte, 1)) {    //发送器件地址+读/写命令
            }
        } else {
            byte = (i2cmodular.addr << 1) | 0X00;
            if(DEVI2C_Write_(&byte, 1)) {    //发送器件地址+写命令
            }
            if(DEVI2C_Write_(&address, 1)) {    //发送寄存器地址
            }
            if(rw) {
                DEVI2C_Start_();
                byte = (i2cmodular.addr << 1) | 0X01;
                if(DEVI2C_Write_(&byte, 1)) {    //发送器件地址+读命令
                }
            }
        }
        if(rw) {
            DEVI2C_Read_(pdata, size);    //连续读取
        } else {
            if(DEVI2C_Write_(pdata, size)) {    //连续写入
            }
        }
        DEVI2C_Stop_();
        return DEV_OK;
    }
}
#undef DEVI2C_SCL_Set
#undef DEVI2C_SDA_Set
#undef DEVI2C_SCL_Out
#undef DEVI2C_SDA_Out
#undef DEVI2C_SCL_In
#undef DEVI2C_SDA_In
#undef DEVI2C_Error
#undef DEVI2C_Delayus
#undef DEVI2C_Delayms
#undef DEVI2C_Delayus_paral

#endif    // DEVI2C_SOFTWARE_ENABLED


/*****   SOFTWARE STRUCTURE DEFINITION & FUNCTION IMPLEMENTATION OF SPI DEVICE COMMUNITCATION   *****/
#ifdef DEVSPI_SOFTWARE_ENABLED
typedef struct {    //SPI模拟总线结构体
    bool something;
#ifdef DEVSPI_USEPOINTER
    void (*SCK_Out)(bool);
    void (*SDI_RXD_Out)(bool);
    bool (*SDO_In)();
    void (*CS_Out)(bool);
    void (*error)(int8_t err);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
    int8_t (*delayus_paral)(uint16_t us);
#endif    //DEVSPI_USEPOINTER
} SPI_SoftHandleTypeDef;
#if defined(DEVSPI_USEPOINTER)
#define DEVSPI_SCK_Out(pot)      ((SPI_SoftHandleTypeDef *)modular->bus)->SCK_Out(pot)
#define DEVSPI_SDI_RXD_Out(pot)  ((SPI_SoftHandleTypeDef *)modular->bus)->SDI_RXD_Out(pot)
#define DEVSPI_SDO_In()          ((SPI_SoftHandleTypeDef *)modular->bus)->SDO_In()
#define DEVSPI_CS_Out(pot)       ((SPI_SoftHandleTypeDef *)modular->bus)->CS_Out(pot)
#define DEVSPI_Error(err)        ((SPI_SoftHandleTypeDef *)modular->bus)->error(err)
#define DEVSPI_delayus(us)       ({if(us) {((SPI_SoftHandleTypeDef *)modular->bus)->delayus(us-TIMER_DELAY);} })
#define DEVSPI_delayms(ms)       ({if(ms) {((SPI_SoftHandleTypeDef *)modular->bus)->delayms(ms);} })
#define DEVSPI_Delayus_paral(us) ((SPI_SoftHandleTypeDef *)modular->bus)->delayus_paral(us)
#else
#define DEVSPI_SCK_Out(pot)      DEVCMNI_SCL_SCK_Out(pot)
#define DEVSPI_SDI_RXD_Out(pot)  DEVCMNI_SDA_SDI_RXD_OWRE_Out(pot)
#define DEVSPI_SDO_In()          DEVCMNI_SDO_In()
#define DEVSPI_CS_Out(pot)       DEVCMNI_CS_Out(pot)
#define DEVSPI_Error(err)        DEVCMNI_Error(err)
#define DEVSPI_delayus(us)       ({if(us) {DEVCMNI_Delayus(us-TIMER_DELAY);} })
#define DEVSPI_delayms(ms)       ({if(ms) {DEVCMNI_Delayms(ms);} })
#define DEVSPI_Delayus_paral(us) DEVCMNI_Delayus_paral(us)
#endif    // DEVSPI_USEPOINTER

static void DEVSPI_Init(SPI_ModuleHandleTypeDef *modular) {}
/* 以下的整个持续读写过程为原子操作, 每个函数(除Start())开始前与(除Stop())结束后总是有: SCK为未释放状态 */
/* 对于进一步切割, 使同一条总线上的读写操作可以并行运行的工作, 还有待探究 */
static void DEVSPI_Start(SPI_ModuleHandleTypeDef *modular, int8_t skip) {
    if(!skip) {    //如果能设置片选, 才初始化总线电位, 否则可能写入多余数据
        DEVSPI_CS_Out(HIGH);
        DEVSPI_SDI_RXD_Out(HIGH);
        DEVSPI_delayus(DEVSPI_SCLK_LOW_TIME);
        DEVSPI_SCK_Out(LOW);
        DEVSPI_CS_Out(LOW);    //拉低片选
    }
}
static void DEVSPI_Stop(SPI_ModuleHandleTypeDef *modular, int8_t skip) {
    if(!skip) {
        DEVSPI_CS_Out(HIGH);    //拉高片选
        DEVSPI_SDI_RXD_Out(HIGH);
        DEVSPI_delayus(DEVSPI_SCLK_HIGH_TIME);
        DEVSPI_SCK_Out(HIGH);
        DEVSPI_delayus(DEVSPI_SCLK_HIGH_TIME);
    }
}
static inline void DEVSPI_WriteBit(SPI_ModuleHandleTypeDef *modular, bool bit) {
    DEVSPI_SDI_RXD_Out(bit);
    DEVSPI_delayus(DEVSPI_SCLK_LOW_TIME);
    DEVSPI_SCK_Out(HIGH);
    DEVSPI_delayus(DEVSPI_SCLK_HIGH_TIME);
    DEVSPI_SCK_Out(LOW);
}
static inline bool DEVSPI_ReadBit(SPI_ModuleHandleTypeDef *modular) {
    bool bit = 0;
    DEVSPI_delayus(DEVSPI_SCLK_LOW_TIME);
    DEVSPI_SCK_Out(HIGH);
    DEVSPI_delayus(DEVSPI_SCLK_HIGH_TIME);
    bit = DEVSPI_SDO_In();
    DEVSPI_SCK_Out(LOW);
    return bit;
}
static inline bool DEVSPI_TransmitBit(SPI_ModuleHandleTypeDef *modular, bool bit) {
    DEVSPI_SDI_RXD_Out(bit);
    DEVSPI_delayus(DEVSPI_SCLK_LOW_TIME);
    DEVSPI_SCK_Out(HIGH);
    DEVSPI_delayus(DEVSPI_SCLK_HIGH_TIME);
    bit = DEVSPI_SDO_In();
    DEVSPI_SCK_Out(LOW);
    return bit;
}

__attribute__((unused)) static DEV_StatusTypeDef DEVSPI_Transmit(SPI_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, bool rw, uint32_t timeout) {
    /* 多字节读写函数, timeout应答超时,rw1为读0为写 */
    DEVSPI_Init(modular);
    DEVSPI_Start(modular, modular->skip);
    if(modular->duplex == DEVSPI_FULL_DUPLEX) {
        for(uint16_t j = 0; j < size; j++) {
            pdata[j] |= DEVSPI_TransmitBit(modular, pdata[j] & 0x80 ? 1 : 0) ? 0x80 : 0x00;
            pdata[j] |= DEVSPI_TransmitBit(modular, pdata[j] & 0x40 ? 1 : 0) ? 0x40 : 0x00;
            pdata[j] |= DEVSPI_TransmitBit(modular, pdata[j] & 0x20 ? 1 : 0) ? 0x20 : 0x00;
            pdata[j] |= DEVSPI_TransmitBit(modular, pdata[j] & 0x10 ? 1 : 0) ? 0x10 : 0x00;
            pdata[j] |= DEVSPI_TransmitBit(modular, pdata[j] & 0x08 ? 1 : 0) ? 0x08 : 0x00;
            pdata[j] |= DEVSPI_TransmitBit(modular, pdata[j] & 0x04 ? 1 : 0) ? 0x04 : 0x00;
            pdata[j] |= DEVSPI_TransmitBit(modular, pdata[j] & 0x02 ? 1 : 0) ? 0x02 : 0x00;
            pdata[j] |= DEVSPI_TransmitBit(modular, pdata[j] & 0x01 ? 1 : 0) ? 0x01 : 0x00;
        }
    } else if(modular->duplex == DEVSPI_HALF_DUPLEX) {
        if(rw) {
            for(uint16_t j = 0; j < size; j++) {
                pdata[j] |= DEVSPI_ReadBit(modular) ? 0x80 : 0x00;
                pdata[j] |= DEVSPI_ReadBit(modular) ? 0x40 : 0x00;
                pdata[j] |= DEVSPI_ReadBit(modular) ? 0x20 : 0x00;
                pdata[j] |= DEVSPI_ReadBit(modular) ? 0x10 : 0x00;
                pdata[j] |= DEVSPI_ReadBit(modular) ? 0x08 : 0x00;
                pdata[j] |= DEVSPI_ReadBit(modular) ? 0x04 : 0x00;
                pdata[j] |= DEVSPI_ReadBit(modular) ? 0x02 : 0x00;
                pdata[j] |= DEVSPI_ReadBit(modular) ? 0x01 : 0x00;
            }
        } else {
            for(uint16_t j = 0; j < size; j++) {
                DEVSPI_WriteBit(modular, pdata[j] & 0x80);
                DEVSPI_WriteBit(modular, pdata[j] & 0x40);
                DEVSPI_WriteBit(modular, pdata[j] & 0x20);
                DEVSPI_WriteBit(modular, pdata[j] & 0x10);
                DEVSPI_WriteBit(modular, pdata[j] & 0x08);
                DEVSPI_WriteBit(modular, pdata[j] & 0x04);
                DEVSPI_WriteBit(modular, pdata[j] & 0x02);
                DEVSPI_WriteBit(modular, pdata[j] & 0x01);
            }
        }
    }
    DEVSPI_Stop(modular, modular->skip);
    return DEV_OK;
}
#undef DEVSPI_SCK_Out
#undef DEVSPI_SDI_RXD_Out
#undef DEVSPI_SDO_In
#undef DEVSPI_CS_Out
#undef DEVSPI_Error
#undef DEVSPI_delayus
#undef DEVSPI_delayms
#undef DEVSPI_Delayus_paral

#endif    // DEVSPI_SOFTWARE_ENABLED


/*****   SOFTWARE IMPLEMENTATION FUNCTION OF UART DEVICE COMMUNITCATION   *****/
#ifdef DEVUART_SOFTWARE_ENABLED

#endif    // DEVUART_SOFTWARE_ENABLED


/*****   SOFTWARE STRUCTURE DEFINITION & FUNCTION IMPLEMENTATION OF 1-WIRE DEVICE COMMUNITCATION   *****/
#ifdef DEVOWRE_SOFTWARE_ENABLED
typedef struct {           //ONEWIRE模拟总线结构体
    uint64_t num;          //总线上的设备数量
    int8_t flag_search;    //总线在最近一段时间内是否进行过搜索以更新设备数量
#ifdef DEVOWRE_USEPOINTER
    void (*OWIO_Set)(bool);
    void (*OWIO_Out)(bool);
    bool (*OWIO_In)(void);
    void (*error)(int8_t err);
    void (*delayus)(uint16_t us);
    void (*delayms)(uint16_t ms);
    int8_t (*delayus_paral)(uint16_t us);
#endif    // DEVOWRE_USEPOINTER
} ONEWIRE_SoftHandleTypeDef;
#if defined(DEVOWRE_USEPOINTER)
#define DEVOWRE_OWIO_Set(dir)     ((ONEWIRE_SoftHandleTypeDef *)modular->bus)->OWIO_Set(dir)
#define DEVOWRE_OWIO_Out(pot)     ((ONEWIRE_SoftHandleTypeDef *)modular->bus)->OWIO_Out(pot)
#define DEVOWRE_OWIO_In()         ((ONEWIRE_SoftHandleTypeDef *)modular->bus)->OWIO_In()
#define DEVOWRE_Error(err)        ((ONEWIRE_SoftHandleTypeDef *)modular->bus)->error(err)
#define DEVOWRE_delayus(us)       ({if(us) {((ONEWIRE_SoftHandleTypeDef *)modular->bus)->delayus(us);} })
#define DEVOWRE_delayms(ms)       ({if(ms) {((ONEWIRE_SoftHandleTypeDef *)modular->bus)->delayms(ms);} })
#define DEVOWRE_Delayus_paral(us) ((ONEWIRE_SoftHandleTypeDef *)modular->bus)->delayus_paral(us)
#else
#define DEVOWRE_OWIO_Set(pot)     DEVCMNI_SDA_OWRE_Set(pot)
#define DEVOWRE_OWIO_Out(pot)     DEVCMNI_SDA_SDI_RXD_OWRE_Out(pot)
#define DEVOWRE_OWIO_In(pot)      DEVCMNI_SDA_OWRE_In(pot)
#define DEVOWRE_Error(err)        DEVCMNI_Error(err)
#define DEVOWRE_delayus(us)       ({if(us) {DEVCMNI_Delayus(us);} })
#define DEVOWRE_delayms(ms)       ({if(ms) {DEVCMNI_Delayms(ms);} })
#define DEVOWRE_Delayus_paral(us) DEVCMNI_Delayus_paral(us)
#endif    // DEVOWRE_USEPOINTER

static inline int8_t DEVOWRE_Init(ONEWIRE_ModuleHandleTypeDef *modular) {
    DEVOWRE_OWIO_Out(HIGH);
    DEVOWRE_OWIO_Set(IN);
    if(DEVOWRE_OWIO_In() != HIGH) {
        return 1;    //若总线没有被释放, 返回错误值
    }
    DEVOWRE_OWIO_Set(OUT);
    return 0;
}
static inline uint8_t DEVOWRE_Reset(ONEWIRE_ModuleHandleTypeDef *modular) {
    uint8_t result = 0;
    DEVOWRE_OWIO_Out(LOW);    //拉低总线480us, 发出复位信号
    DEVOWRE_delayus(480);
    //todo: 关中断      //应答信号最好在60-120us期间读取, 应关闭中断
    DEVOWRE_OWIO_Out(HIGH);    //释放总线, 等待60us
    DEVOWRE_delayus(60);
    DEVOWRE_OWIO_Set(IN);              //读取总线信息
    if(DEVOWRE_OWIO_In() == HIGH) {    //若总线被拉低, 返回0, 否则返回1
        result = 1;
    } else {
        result = 0;
    }
    //todo: 开中断
    DEVOWRE_delayus(420);
    DEVOWRE_OWIO_Set(OUT);
    return result;
}
static inline void DEVOWRE_WriteBit(ONEWIRE_ModuleHandleTypeDef *modular, bool bit) {
    if(bit) {
        //todo: 关中断
        DEVOWRE_OWIO_Out(LOW);
        DEVOWRE_delayus(5);
        DEVOWRE_OWIO_Out(HIGH);
        //todo: 开中断
        DEVOWRE_delayus(55);
    } else {
        DEVOWRE_OWIO_Out(LOW);
        DEVOWRE_delayus(60);
        DEVOWRE_OWIO_Out(HIGH);
        DEVOWRE_delayus(1);
    }
}
static inline uint8_t DEVOWRE_ReadBit(ONEWIRE_ModuleHandleTypeDef *modular) {
    bool bit = 0;
    //todo: 关中断
    DEVOWRE_OWIO_Out(LOW);     //拉低总线1us, 开始读时隙
    DEVOWRE_delayus(1);        //若主控的主频较低, 可跳过1us的延时
    DEVOWRE_OWIO_Out(HIGH);    //释放总线
    DEVOWRE_OWIO_Set(IN);
    DEVOWRE_delayus(10);    //在15us内的最后时刻读取总线, 考虑到误差只延时10us
    if(DEVOWRE_OWIO_In() == HIGH) {
        bit = 1;
    } else {
        bit = 0;
    }
    //todo: 开中断
    if(bit) {
        DEVOWRE_delayus(50);
    } else {
        DEVOWRE_delayus(51);
    }
    DEVOWRE_OWIO_Set(OUT);
    return bit;
}
static inline void DEVOWRE_WriteByte(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t byte) {
    for(uint8_t i = 0x01; i; i <<= 1) {
        DEVOWRE_WriteBit(modular, byte & i ? 1 : 0);
    }
}
static inline uint8_t DEVOWRE_ReadByte(ONEWIRE_ModuleHandleTypeDef *modular) {
    uint8_t byte = 0;
    for(uint8_t i = 0x01; i; i <<= 1) {
        byte |= DEVOWRE_ReadBit(modular) ? i : 0x00;
    }
    return byte;
}
static inline void DEVOWRE_Write(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x01; i; i <<= 1) {
            DEVOWRE_WriteBit(modular, pdata[j] & i ? 1 : 0);
        }
    }
    return;
}
static inline void DEVOWRE_Read(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size) {
    for(uint16_t j = 0; j < size; j++) {
        for(uint8_t i = 0x01; i != 0; i <<= 1) {
            pdata[j] |= DEVOWRE_ReadBit(modular) ? i : 0x00;
        }
    }
    return;
}

#define _SKIP        0xCC    //ROM跳过匹配指令[限单个设备/接多个设备能同时进行的操作]
#define _MATCH       0x55    //ROM匹配指令
#define _QUERY       0x33    //ROM读取指令[限单个设备]
#define _SEARCH      0xF0    //ROM搜索指令
#define _ALARMSEARCH 0xEC    //ROM报警搜索指令
/**
 * @description: 单总线协议ROM搜索/报警搜索操作
 * 在发起一个复位信号后, 根据总线设备数量, 发出ROM读取/搜索指令, 或发出ROM报警搜索指令
 * @param {ONEWIRE_ModuleHandleTypeDef} *modular
 * @param {int8_t} searchtype
 * @return {*}
 */
__attribute__((unused)) static void DEVONEWIRE_Search(ONEWIRE_ModuleHandleTypeDef *modular, int8_t searchtype) {
    DEVOWRE_Init(modular);
    DEVOWRE_Reset(modular);
    if(searchtype == 0) {
        if(((ONEWIRE_SoftHandleTypeDef *)modular->bus)->num == 1) {
            DEVOWRE_WriteByte(modular, _QUERY);
        } else if(((ONEWIRE_SoftHandleTypeDef *)modular->bus)->num > 1) {
            DEVOWRE_WriteByte(modular, _SEARCH);
            //...
        }
    } else {
        DEVOWRE_WriteByte(modular, _ALARMSEARCH);
        //...
    }
}
__attribute__((unused)) static DEV_StatusTypeDef DEVONEWIRE_ReadBit(ONEWIRE_ModuleHandleTypeDef *modular, bool *bit) {
    *bit = DEVOWRE_ReadBit(modular);
    return DEV_OK;
}
__attribute__((unused)) static DEV_StatusTypeDef DEVONEWIRE_Write(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, uint32_t timeout) {
    DEVOWRE_Init(modular);
    DEVOWRE_Reset(modular);
    if(modular->skip || ((ONEWIRE_SoftHandleTypeDef *)modular->bus)->num == 1) {
        DEVOWRE_WriteByte(modular, _SKIP);
    } else if(((ONEWIRE_SoftHandleTypeDef *)modular->bus)->num > 1) {
        DEVOWRE_WriteByte(modular, _MATCH);
        DEVOWRE_Write(modular, (uint8_t *)&modular->rom, 8);
    }
    DEVOWRE_Write(modular, pdata, size);
    return DEV_OK;
}
__attribute__((unused)) static DEV_StatusTypeDef DEVONEWIRE_Read(ONEWIRE_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size) {
    //tofix: 单总线的读时隙只能跟随在特定的主机写指令之后吗? 当需要并发地与多个设备进行通信时, 怎样进行独立的读操作?
    DEVOWRE_Read(modular, pdata, size);
    return DEV_OK;
}
#undef DEVOWRE_OWIO_Set
#undef DEVOWRE_OWIO_Out
#undef DEVOWRE_OWIO_In
#undef DEVOWRE_Error
#undef DEVOWRE_delayus
#undef DEVOWRE_delayms
#undef DEVOWRE_Delayus_paral
#endif    // DEVOWRE_SOFTWARE_ENABLED


/*****   HARDWARE IMPLEMENTATION FUNCTION OF I2C DEVICE COMMUNITCATION   *****/
#if defined(DEVI2C_HARDWARE_ENABLED)
#include "i2c.h"
DEV_StatusTypeDef DEVI2C_Transmit_H(I2C_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, uint8_t address, bool rw, uint32_t timeout);

#endif    // DEVI2C_HARDWARE_ENABLED


/*****   HARDWARE IMPLEMENTATION FUNCTION OF SPI DEVICE COMMUNITCATION   *****/
#if defined(DEVSPI_HARDWARE_ENABLED)
#include "spi.h"
DEV_StatusTypeDef DEVSPI_Transmit_H(SPI_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, bool rw, uint32_t timeout);

#endif    // DEVSPI_HARDWARE_ENABLED


/*****   HARDWARE IMPLEMENTATION FUNCTION OF UART DEVICE COMMUNITCATION   *****/
#if defined(DEVUART_HARDWARE_ENABLED)
#include "usart.h"
/* 串口接收函数 */
DEV_StatusTypeDef DEVUART_Receive(UART_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size, size_t *length);
/* 串口发送函数 */
DEV_StatusTypeDef DEVUART_Transmit(UART_ModuleHandleTypeDef *modular, uint8_t *pdata, size_t size);

#endif    // DEVUART_HARDWARE_ENABLED


/*****   HARDWARE IMPLEMENTATION FUNCTION OF 1-WIRE DEVICE COMMUNITCATION   *****/
#if defined(DEVOWRE_HARDWARE_ENABLED)
#include "onewire.h"

#endif    // DEVOWRE_HARDWARE_ENABLED

#endif    // !__DEVICE_PROTOCOL_H
