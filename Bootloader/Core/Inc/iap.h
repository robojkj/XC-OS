/*
*******************************************************************************************************
*
* 文件名称 : main.c
* 版    本 : V1.0
* 作    者 : OpenRabbit
* 说    明 : 主函数文件
* 
*******************************************************************************************************
*/



/* 头文件 -----------------------------------------------------------*/
#include "stm32f4xx_hal.h"


/* 宏定义 -----------------------------------------------------------*/

/* 第一个应用程序起始地址(存放在FLASH) */
/* 保留0X08000000~0X0800FFFF的空间为Bootloader使用(共64KB) */
#define APPLICATION_ADDRESS   (uint32_t)0x08010000 

typedef  void (*pFunction)(void);

/* 变量 -------------------------------------------------------------*/


/* 函数声明 ---------------------------------------------------------*/

void iap_schedule(void);
unsigned char CRC_Check(unsigned char *buffer);
unsigned int CRC16 ( unsigned char *arr_buff, unsigned char len);




/***************************** 跳兔科技 www.whtiaotu.com (END OF FILE) *********************************/
