#include "device.h"
#include "protocol_software.h"
//tofix: ac5编译不支持__weak函数在别处定义为inline
__weak void DEVCMNI_SCL_Set(bool dir) {}
__weak void DEVCMNI_SDA_OWRE_Set(bool dir) {}
__weak void DEVCMNI_SCL_SCK_Out(bool pot) {}
__weak void DEVCMNI_SDA_SDI_OWRE_Out(bool pot) {}
__weak bool DEVCMNI_SCL_In(void) { return HIGH; }
__weak bool DEVCMNI_SDA_OWRE_In(void) { return HIGH; }
__weak bool DEVCMNI_SDO_In(void) { return HIGH; }
__weak void DEVCMNI_CS_Out(bool pot) {}
__weak void DEVCMNI_Error(int8_t err) {}
__weak void DEVCMNI_Delayus(uint64_t us) {}
__weak void DEVCMNI_Delayms(uint64_t ms) {}
__weak int8_t DEVCMNI_Delayus_paral(uint64_t us) { return 1; }
