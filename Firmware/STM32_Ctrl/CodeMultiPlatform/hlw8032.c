#include "hlw8032.h"

/* 最坏情况: 先读23位无效数据, 再读24位有效数据 */
#define lenof(arr)       (sizeof(arr) / sizeof(*arr))
#define HLW8032_BUF_SIZE 50
static uint8_t buffer[HLW8032_BUF_SIZE];

/* 结果数组中保存的是四位小数的定点数 */
HLW8032_ResTypedef pfcres = {0};

bool HLW8032_Read(uint8_t *data, size_t size);
static bool HLW8032_Check(uint8_t *data, size_t size, uint8_t *start);
static void HLW8032_Count(uint8_t *data);
static void HLW8032_Limit(uint8_t *data);

bool HLW8032_Ctrl(void) {
    bool res = false;
    uint8_t start = 0;
    do {
        res = HLW8032_Read(buffer, lenof(buffer));
        if(!res) { break; }
        res = HLW8032_Check(buffer, lenof(buffer), &start);
        if(!res) { break; }
        HLW8032_Count(&buffer[start]);
        HLW8032_Limit(&buffer[start]);
    } while(0);
    return res;
}

HLW8032_ResTypedef *HLW8032_GetResult(void) {
    return &pfcres;
}

__weak bool HLW8032_Read(uint8_t *data, size_t size) {
    /* user should rewrite this function to read data for hlw8032 */
    printf("user should rewrite this function: HLW8032_Read to read data for hlw8032\r\n");
    return false;
}

/* 校验串口接收的数据 */
static bool HLW8032_Check(uint8_t *data, size_t size, uint8_t *start) {
    size_t i = 0, s = 0;
    int8_t state = 0;
    uint8_t sumCheck = 0;
    for(i = 0; i < lenof(buffer); i++) {
        switch(state) {
        case 0:
            if(buffer[i] == 0x55 || buffer[i] == 0xf2) {
                state = 1;
            }
            break;
        case 1:
            if(buffer[i] == 0x5A) {
                s = i - 1;
                state = 2;
            } else {
                if(buffer[i] == 0x55 || buffer[i] == 0xf2) {
                    state = 1;
                } else {
                    state = 0;
                }
            }
            break;
        case 2:
            if(i < s + 23) {
                sumCheck += buffer[i];
            } else {
                if(sumCheck == buffer[i]) {
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

static void HLW8032_Count(uint8_t *data) {
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
    pfcres.voltage = DEC_BIT * Voltage_Parameter_REG * 3.006 / Voltage_REG;
    /* 计算电流 */
    pfcres.currentIntensity = DEC_BIT * Current_Parameter_REG / Current_REG;
    /* 计算功率 */
    pfcres.power = DEC_BIT * Power_Parameter_REG * 3.006 / Power_REG;
    /* 计算功率因数 */
    pfcres.powerFactorer = DEC_BIT * (pfcres.power / DEC_BIT) / ((pfcres.voltage / DEC_BIT) * (pfcres.currentIntensity / DEC_BIT));
    /* 计算电能 */
    pfcres.electricQuantity = DEC_BIT * Energy_CNT / 1000000000 * Power_Parameter_REG * 3.006 * 1 / 3600;
}

static void HLW8032_Limit(uint8_t *data) {
    /* 若电压小于2V, 则视为假值, 不计算功率因数 */
    if(pfcres.voltage < 2 * DEC_BIT && pfcres.currentIntensity < 0.1 * DEC_BIT) {
        // pfcres.voltage = 0;
        // pfcres.currentIntensity;
        pfcres.powerFactorer = 0;
    }
    /* 若功率溢出且功率未更新，归0 */
    if((data[0] == 0xf2) && (((data[20] >> 4) & 1) == 0)) {
        pfcres.power = 0;
        pfcres.powerFactorer = 0;
    }
    /* 将功率因数限幅在1以内 */
    if(pfcres.powerFactorer >= 1 * DEC_BIT) {
        pfcres.powerFactorer = 1 * DEC_BIT - 1;
    }

    /* 由于有时存在脏值，将结果限幅为三位整数四位小数的定点数 */
    uint32_t *p = (uint32_t *)&pfcres;
    for(size_t i = 0; i < sizeof(pfcres) / sizeof(*p); i++) {
        p[i] %= (uint32_t)(INT_BIT * DEC_BIT);
    }
}
