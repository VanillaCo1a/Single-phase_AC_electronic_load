#ifndef __HLW8032_H
#define __HLW8032_H
#include "device.h"

/* 定点数的整数和小数长度 */
#define INT_BIT 1000
#define DEC_BIT 10000.0

typedef struct {
    uint32_t voltage;
    uint32_t currentIntensity;
    uint32_t power;
    uint32_t powerFactorer;
    uint32_t electricQuantity;
} HLW8032_ResTypedef;

bool HLW8032_Ctrl(void);
HLW8032_ResTypedef *HLW8032_GetResult(void);

#endif // !__HLW8032_H
