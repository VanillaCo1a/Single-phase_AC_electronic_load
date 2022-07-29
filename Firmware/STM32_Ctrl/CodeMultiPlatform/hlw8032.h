#ifndef __HLW8032_H
#define __HLW8032_H
#include "device.h"

typedef struct {
    uint32_t voltage;
    uint32_t currentIntensity;
    uint32_t power;
    uint32_t powerFactorer;
    uint32_t electricQuantity;
} HLW8032_ResTypedef;

void HLW8032_Init(void);
bool HLW8032_Ctrl(void);
HLW8032_ResTypedef *HLW8032_GetResult(void);
void HLW8032_Limit(void);

void HLW8032_Task(void);
#endif // !__HLW8032_H
