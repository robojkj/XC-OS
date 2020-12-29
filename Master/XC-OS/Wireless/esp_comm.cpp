#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "esp_comm.h"


typedef struct
{
    uint8_t step;
    uint32_t timeout;
    uint8_t temp;
    uint32_t baud;
    u8 rxfinish;
} _esp_Protocol_TypeDef;

static _esp_Protocol_TypeDef esp_tool_prl,  esp_update_prl;


USART_TypeDef*  _get_UARTx[6] = { USART1, USART2, USART3, UART4, UART5, USART6 };



void change_uart_baud(uint8_t id, uint32_t baud)
{
    float temp;
    uint16_t mantissa;
    uint16_t fraction;
    uint32_t pclk;
    USART_TypeDef *UARTx;
    RCC_ClocksTypeDef RCC_ClocksStatus;

    RCC_GetClocksFreq(&RCC_ClocksStatus);
    if(id == 1 || id == 6)
    {
        pclk = RCC_ClocksStatus.PCLK2_Frequency;
    }
    else
    {
        pclk = RCC_ClocksStatus.PCLK1_Frequency;
    }

    temp = (float)pclk / (baud * 16); //得到USARTDIV
    mantissa = temp;             //得到整数部分
    fraction = (temp - mantissa) * 16; //得到小数部分
    mantissa <<= 4;
    mantissa += fraction;

    UARTx = _get_UARTx[id - 1];

    //波特率设置
    UARTx->BRR = mantissa; // 波特率设置
}





void esp_tool_protocol(uint8_t inD)
{
    _esp_Protocol_TypeDef *p = &esp_tool_prl;
    if(millis() - p->timeout > 5)
    {
        p->timeout = millis();
        p->step = 0;
    }

    switch(p->step)
    {
    case 0:
        if(inD == 0xC0) p->step++;
        else p->step = 0;
        break;
    case 1:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 2:
        if(inD == 0x0F) p->step++;
        else p->step = 0;
        break;
    case 3:
        if(inD == 0x08) p->step++;
        else p->step = 0;
        break;
    case 4:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 5:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 6:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 7:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 8:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 9:
        p->baud = inD;
        p->step++;
        break;
    case 10:
        p->baud += inD << 8;
        p->step++;
        break;
    case 11:
        p->baud += inD << 16;
        p->step++;
        break;
    case 12:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 13:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 14:
        if(inD == 0xC2) p->step++;
        else p->step = 0;
        break;
    case 15:
        if(inD == 0x01) p->step++;
        else p->step = 0;
        break;
    case 16:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 17:
        if(inD == 0xC0)
        {
            p->step = 0;
            esp_update_prl.baud = p->baud;
        }
        break;
    }
}

void esp_update_protocol(uint8_t inD)
{
    _esp_Protocol_TypeDef *p = &esp_update_prl;
    if(millis() - p->timeout > 3)
    {
        p->timeout = millis();
        p->step = 0;
    }

    switch(p->step)
    {
    case 0:
        if(inD == 0x01) p->step++;
        else p->step = 0;
        break;
    case 1:
        if(inD == 0x0F || 0x12)
        {
            p->step++;
            p->temp = inD;
        }
        else p->step = 0;
        break;
    case 2:
        if(inD == 0x02) p->step++;
        else p->step = 0;
        break;
    case 3:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 4:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 5:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 6:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 7:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 8:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 9:
        if(inD == 0x00) p->step++;
        else p->step = 0;
        break;
    case 10:
        if(inD == 0xC0)
        {
            p->step = 0;
            p->rxfinish = 1;
        }
        break;
    }
}

//必须运行在线程上
void ESP_Dispose()
{
    _esp_Protocol_TypeDef *p = &esp_update_prl;
    
    if(p->rxfinish)
    {
        p->rxfinish = 0;
        if(p->temp == 0x0F)
        {
            vTaskDelay(3);
            change_uart_baud(1, p->baud);
            change_uart_baud(2, p->baud);
        }
        else if(p->temp == 0x12)
        {
            vTaskDelay(1);
            //NVIC_SystemReset();
            change_uart_baud(1, 115200);
            change_uart_baud(2, 115200);
        }
    }
}














