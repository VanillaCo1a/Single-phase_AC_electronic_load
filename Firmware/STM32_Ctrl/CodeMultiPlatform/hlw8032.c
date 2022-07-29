#include "device.h"
#include "uart.h"
#include "hlw8032.h"

// #define HLW8032_NUM 1
// extern DEVCMNI_TypeDef uart_cmni[];
// DEVS_TypeDef hlw8032s = {.type = HLW8032};
// DEV_TypeDef hlw8032[HLW8032_NUM] = {
//     {.parameter = NULL, .io = {0}, .cmni = {.num = 1, .confi = (DEVCMNI_TypeDef *)&uart_cmni[1], .init = NULL}},
// };

// void HLW8032_Init(void) {
//     //DEV_Init(&hlw8032s, hlw8032, sizeof(hlw8032) / sizeof(*hlw8032));
//     printf("hlw8032初始化完毕\r\n");
// }
// // bool UART3_ScanArray(uint8_t arr[], size_t size, size_t *length) {
// //     DEV_setActStream(&hlw8032s, 0);
// //     return DEVCMNI_Read((uint8_t *)arr, size, length, 0xFF);
// // }

/* 最坏情况: 先读23位无效数据, 再读24位有效数据 */
#define _SIZE_MAX 50
static uint8_t buffer[_SIZE_MAX];

/* 结果数组中保存的是三位小数的定点数 */
HLW8032_ResTypedef pfcres = {0};

static bool ctrl(uint8_t *data, size_t size, uint8_t *start);
static void count(uint8_t *data);

bool HLW8032_Ctrl(void) {
    bool res = false;
    size_t size;
    uint8_t start = 0;
    if(UART3_ScanArray(buffer, sizeof(buffer), &size)) {
        if((res = ctrl(buffer, _SIZE_MAX, &start))) {
            count(&buffer[start]);
        }
    }
    return res;
}

/* 由于有时存在脏值，将结果限幅为3位整数3位小数的定点数 */
void HLW8032_Limit(void) {
    uint32_t *p = (uint32_t *)&pfcres;
    for(size_t i = 0; i < sizeof(pfcres) / sizeof(*p); i++) {
        p[i] %= 1000000;
    }
}

static bool ctrl(uint8_t *data, size_t size, uint8_t *start) {
    size_t i = 0, s = 0;
    int8_t state = 0;
    uint8_t sumCheck = 0;
    for(i = 0; i < size; i++) {
        switch(state) {
        case 0:
            if(data[i] == 0x55 || data[i] == 0xf2) {
                state = 1;
            }
            break;
        case 1:
            if(data[i] == 0x5A) {
                s = i - 1;
                state = 2;
            } else {
                if(data[i] == 0x55 || data[i] == 0xf2) {
                    state = 1;
                } else {
                    state = 0;
                }
            }
            break;
        case 2:
            if(i < s + 23) {
                sumCheck += data[i];
            } else {
                if(sumCheck == data[i]) {
                    *start = s;
                    return true;
                } else {
                    *start = 0;
                    return false;
                }
            }
            break;
        default:
            break;
        }
    }

    *start = 0;
    return false;
}

/* 定点数的小数长度 */
#define NUMBIT 1000.0

static void count(uint8_t *data) {
    unsigned long Voltage_Parameter_REG = 0;    //电压参数寄存器
    unsigned long Voltage_REG = 0;              //电压寄存器
    unsigned long Current_Parameter_REG = 0;    //电压参数寄存器
    unsigned long Current_REG = 0;              //电压寄存器
    unsigned long Power_Parameter_REG = 0;      //电压参数寄存器
    unsigned long Power_REG = 0;                //电压寄存器
    unsigned char DATA_REG_BIT7 = 0;            //数据更新寄存器BIT7
    unsigned int Energy_count = 0;              //
    unsigned long Energy_CNT = 0;               //

    Voltage_Parameter_REG = data[2] * 65536 + data[3] * 256 + data[4];
    Voltage_REG = data[5] * 65536 + data[6] * 256 + data[7];
    Current_Parameter_REG = data[8] * 65536 + data[9] * 256 + data[10];
    Current_REG = data[11] * 65536 + data[12] * 256 + data[13];
    Power_Parameter_REG = data[14] * 65536 + data[15] * 256 + data[16];
    Power_REG = data[17] * 65536 + data[18] * 256 + data[19];
    if(DATA_REG_BIT7 != ((data[20] >> 7) & 1)) {
        Energy_count++;
        DATA_REG_BIT7 = (data[20] >> 7) & 1;
    }
    Energy_CNT = Energy_count * 65536 + data[21] * 256 + data[22];

    /* 计算电压 */
    pfcres.voltage = NUMBIT * Voltage_Parameter_REG * 3.006 / Voltage_REG;
    /* 计算电流 */
    pfcres.currentIntensity = NUMBIT * Current_Parameter_REG / Current_REG;
    /* 计算功率 */
    pfcres.power = NUMBIT * Power_Parameter_REG * 3.006 / Power_REG;
    /* 计算功率因数 */
    pfcres.powerFactorer = NUMBIT * (pfcres.power / NUMBIT) / ((pfcres.voltage / NUMBIT) * (pfcres.currentIntensity / NUMBIT));
    /* 计算电能 */
    pfcres.electricQuantity = NUMBIT * Energy_CNT / 1000000000 * Power_Parameter_REG * 3.006 * 1 / 3600;

    /* 若电压小于2V, 则视为假值并归0 */
    if(pfcres.voltage / 1000 < 2) {
        pfcres.voltage = 0;
    }
    /* 若功率溢出且功率未更新，归0 */
    if((data[0] == 0xf2) && (((data[20] >> 4) & 1) == 0)) {
        pfcres.power = 0;
        pfcres.powerFactorer = 0;
    }
}
