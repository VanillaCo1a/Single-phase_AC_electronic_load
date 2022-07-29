#include "myoled.h"
#include "oledlib.h"

/***  OLED配置结构体初始化  ***/
void DEVIO_InitCallBack(void);
/* OLED参数配置 */
static OLED_PARTypeDef oled_parameter[OLED_NUM] = {
    {.chip = OLED_SSD1306, .flip = false}};
/* OLEDIO配置 */
static OLED_IOTypeDef oled_io[OLED_NUM] = {
#if defined(STM32HAL)
    {{.SCL_SCK = {BOARD_OLED_SPI2_SCK_GPIO_Port, BOARD_OLED_SPI2_SCK_Pin},
      .SDA_SDI_OWRE = {BOARD_OLED_SPI2_SDI_GPIO_Port, BOARD_OLED_SPI2_SDI_Pin},
      .CS = {BOARD_OLED_CS_GPIO_Port, BOARD_OLED_CS_Pin}},
     .DC = {BOARD_OLED_DC_GPIO_Port, BOARD_OLED_DC_Pin},
     .RST = {NULL, 0x00}},
#elif defined(STM32FWLIBF1)
    {{.SCL_SCK = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_12},
      .SDA_SDI_OWRE = {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_13}}},
#endif
};
/* OLED通信配置 */
static I2C_SoftHandleTypeDef ahi2c = {.clockstretch = true, .arbitration = true};
static SPI_SoftHandleTypeDef ahspi = {};
static I2C_ModuleHandleTypeDef mi2c[] = {
    {.addr = OLED_I2CADDR1, .skip = false, .speed = DEVI2C_HIGHSPEED, .errhand = DEVI2C_LEVER1}};
static SPI_ModuleHandleTypeDef mspi[] = {{.skip = false, .duplex = DEVSPI_HALF_DUPLEX}};
static OLED_CMNITypeDef oled_cmni[] = {
    {{.protocol = SPI, .ware = SOFTWARE, .bus = &ahspi, .modular = mspi}},
    {{.protocol = SPI, .ware = HARDWARE, .bus = &hspi2, .modular = mspi}}};
/* OLED设备结构体 */
#define SIZE_OLEDIO   sizeof(OLED_IOTypeDef) / sizeof(DEVIO_TypeDef)
#define SIZE_OLEDCMNI sizeof(OLED_CMNITypeDef) / sizeof(DEVCMNI_TypeDef)
DEVS_TypeDef myoleds = {.type = OLED};
DEV_TypeDef myoled[OLED_NUM] = {
    {.parameter = &oled_parameter[0],
     .io = {.num = SIZE_OLEDIO, .confi = (DEVIO_TypeDef *)&oled_io[0], .init = DEVIO_InitCallBack},
     .cmni = {.num = SIZE_OLEDCMNI, .confi = (DEVCMNI_TypeDef *)&oled_cmni[1], .init = NULL}}};
