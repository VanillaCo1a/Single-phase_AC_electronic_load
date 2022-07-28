#ifndef __HLW8032_H
#define __HLW8032_H

typedef struct {
    uint32_t voltage;
    uint32_t currentIntensity;
    uint32_t power;
    uint32_t powerFactorer;
    uint32_t electricQuantity;
} HLW8032_ResTypedef;
extern HLW8032_ResTypedef pfcres;

void HLW8032_Init(void);
bool HLW8032_Ctrl(void);
void HLW8032_Limit(void);

#endif // !__HLW8032_H
