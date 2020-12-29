#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

/***************** 基本设置 *****************/
#define XC_VERSION_HW               "v2.1"
#define XC_VERSION_SW               "v3.1"
#define XC_OS_NAME                  "XC-OS"
#define XC_CPU_NAME                 "STM32F429VGT6"
#define XC_CPU_RATE_MHZ             (216)
#define XC_KB(b)                    ((b)*1024)
#define XC_ROM_SIZE                 (XC_KB(1024))
#define XC_RAM_SIZE                 (XC_KB(192))

#define XC_USE_LUA                  0
#define XC_TIM_WAVPLAYER            TIM8
#define XC_TIM_RUNTIME              TIM7
#define XC_TIM_FFT                  TIM3

#define XC_BATTERY_NAME             "Li-ion 3.7V"
#define XC_BATTERY_VOLTAGE          3700
#define XC_BATTERY_CAPACITY_MAH     2000
#define XC_BATTERY_CURRENT_MAX      3000
#define XC_BATTERY_VOLTAGE_MAX      4200
#define XC_BATTERY_VOLTAGE_MIN      2800

/***************** GPIO设置 *****************/
/*空闲脚*/
#define NULL_PIN PD0

/*显示器*/
#define TFT_D0_Pin   PD14
#define TFT_D1_Pin   PD15
#define TFT_D2_Pin   PD0
#define TFT_D3_Pin   PD1
#define TFT_D4_Pin   PE7
#define TFT_D5_Pin   PE8
#define TFT_D6_Pin   PE9
#define TFT_D7_Pin   PE10
#define TFT_D8_Pin   PE11
#define TFT_D9_Pin   PE12
#define TFT_D10_Pin  PE13
#define TFT_D11_Pin  PE14
#define TFT_D12_Pin  PE15
#define TFT_D13_Pin  PD8
#define TFT_D14_Pin  PD9
#define TFT_D15_Pin  PD10
#define TFT_RST_Pin  PD13
#define TFT_CS_Pin   PD7
#define TFT_RS_Pin   PD11
#define TFT_RW_Pin   PD5
#define TFT_RD_Pin   PD4
#define TFT_LED_Pin  PD12


/*触摸*/
#define Touch_SCL_Pin   PB10
#define Touch_SDA_Pin   PB11
#define Touch_INT_Pin   PB9


/*通信*/    /*与SPI FLASH共用SPI接口*/
#define NRF_MOSI_Pin    PB15
#define NRF_MISO_Pin    PB14
#define NRF_SCK_Pin     PB13
#define NRF_CE_Pin      PB8
#define NRF_CSN_Pin     PB12
#define NRF_IRQ_Pin     PE0
#define FLASH_CS_Pin    PA15
//IRQ   MISO
//MOSI  SCK
//CSN   CE
//VCC   GND
#define ESP_DOWN_Pin	PE2
#define ESP_CS_Pin      PB0
#define ESP_RST_Pin     PB1
#define ESP_CLK_Pin     PB3
#define ESP_MISO_Pin    PB4
#define ESP_MOSI_Pin    PB5
#define ESP_TX_Pin      PA2
#define ESP_RX_Pin      PA3


/*I2C通信总线*/ /**/
#define I2C_SCL_Pin     PB6
#define I2C_SDA_Pin     PB7
#define CHG_KEY_Pin     PE1


/*外置存储器*/

#define SD_CD_Pin       PC4
#define SDIO_D0_Pin     PC8
#define SDIO_D1_Pin     PC9
#define SDIO_D2_Pin     PC10
#define SDIO_D3_Pin     PC11
#define SDIO_SCK_Pin    PC12
#define SDIO_CMD_Pin    PD2

#define SD_CS_Pin       SDIO_D3_Pin
#define SD_SCK_Pin      SDIO_SCK_Pin
#define SD_MOSI_Pin     SDIO_CMD_Pin
#define SD_MISO_Pin     SDIO_D0_Pin




/*其他模块*/
#define Audio_DAC1_Pin  PA4
#define Audio_DAC2_Pin  PA5
#define Audio_SHND_Pin  PA6
#define Audio_MUTE_Pin  PA7
#define SpeakerA_Pin    PA4
#define SpeakerB_Pin    PA5

#define PWR_DEV_Pin     PE3
#define PWR_KEY_Pin     PC2

#define Motor_SLP_Pin   PC5
#define Motor_IN1_Pin   PA0
#define Motor_IN2_Pin   PA1

#define Motor_DIR       1


#endif


