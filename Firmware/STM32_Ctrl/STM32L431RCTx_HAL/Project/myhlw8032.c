#include "myhlw8032.h"
#include "myuart.h"

bool HLW8032_Read(uint8_t *data, size_t size) {
    size_t len;
    return UART3_ScanArray(data, size, &len);
}
