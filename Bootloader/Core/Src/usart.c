/*
*******************************************************************************************************
*
* 文件名称 : usart.c
* 版    本 : V1.0
* 作    者 : OpenRabbit
* 说    明 : 串口驱动文件
* 
*******************************************************************************************************
*/


/* 头文件 -----------------------------------------------------------*/
#include "usart.h"
#include <stdio.h>



/* 变量 -------------------------------------------------------------*/

UART_HandleTypeDef huart1;



/**
  * @brief USART初始化
  * @param None
  * @retval	None
  * @note 主要完成结构体配置
  */
void usart_init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 460800;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}


/**
  * @brief USART底层初始化函数
  * @param None
  * @retval	None
  * @note 该函数会被HAL_xxx_Init()调用,主要完成时钟使能,IO配置
  */
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10    ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  }
}


/**
  * @brief USART底层去初始化函数
  * @param None
  * @retval	None
  * @note 该函数会被HAL_xxx_DeInit()调用,主要完成时钟禁能,IO配置清除
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10    ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

  }
} 


/**
  * @brief  将C库printf函数重新定位到USART
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
	
	while((USART1->SR&USART_SR_TXE)==0);
	USART1->DR = ch;
	while((USART1->SR&USART_SR_TC)==0) {};
	
  return ch;
}  


/***************************** 跳兔科技 www.whtiaotu.com (END OF FILE) *********************************/
