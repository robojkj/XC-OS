#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "BSP.h"
#include "esp_comm.h"

TaskHandle_t TaskHandle_Wireless = 0;

typedef struct
{
    u8 step;
    u32 timeout;
    u8 temp;
    u8 order;
    u8 rxfinish;
} _comm_Protocol_TypeDef;

static _comm_Protocol_TypeDef comm_prl;

void Comm_Stream(u8 inD)
{
    _comm_Protocol_TypeDef *p = &comm_prl;
    if(millis() - p->timeout > 5)
    {
        p->timeout = millis();
        p->step = 0;
    }
    
    switch(p->step)
    {
    case 0:
        if(inD == 'X') p->step++;
        else p->step = 0;
        break;
    case 1:
        if(inD == 'C') p->step++;
        else p->step = 0;
        break;
    case 2:
        p->temp = inD;
        p->step++;
        break;
    case 3:
        if(inD == '\r') p->step++;
        else p->step = 0;
        break;
    case 4:
        if(inD == '\n')
        {
            p->step = 0;
            p->order = p->temp;
            p->rxfinish = 1;
        }
        break;
    }
}

u8 Comm_mode = 0;
void Comm_Response()
{
    if(comm_prl.rxfinish)
    {
        comm_prl.rxfinish = 0;
        if(comm_prl.order == '1')
        {
            Serial.println("OK CommMode");
            Comm_mode = 0;
        }else if(comm_prl.order == '2')
        {
            Serial.println("OK ESP32CommMode");
            Comm_mode = 1;
            digitalWrite(ESP_RST_Pin, LOW);
            digitalWrite(ESP_DOWN_Pin, HIGH);
            vTaskDelay(100);
            digitalWrite(ESP_RST_Pin, HIGH);
        }else if(comm_prl.order == '3')
        {
            Serial.println("OK ESP32UpdateMode");
            Comm_mode = 2;
            digitalWrite(ESP_RST_Pin, LOW);
            digitalWrite(ESP_DOWN_Pin, LOW);
            vTaskDelay(100);
            digitalWrite(ESP_RST_Pin, HIGH);
        }
    }
}

void Serial_EventHandler()
{
    u8 data = Serial.read();
    
    Comm_Stream(data);
    
    if(Comm_mode == 0)
    {
        
    }else if(Comm_mode == 1)
    {
        Serial2.write(data);
    }else if(Comm_mode == 2)
    {
        Serial2.write(data);
        esp_tool_protocol(data);
    }
}

void Serial2_EventHandler()
{
    u8 data = Serial2.read();
    
    if(Comm_mode == 1)
    {
        Serial.write(data);
    }else if(Comm_mode == 2)
    {
        Serial.write(data);
        esp_update_protocol(data);
    }
}

void Task_Wireless(void *pvParameters)
{
	__ExecuteOnce((
        Serial2.begin(115200),
        Serial.attachInterrupt(Serial_EventHandler),
        Serial2.attachInterrupt(Serial2_EventHandler),
        
        change_uart_baud(1, 115200),
        change_uart_baud(2, 115200),
        
        pinMode(ESP_RST_Pin, OUTPUT),
        digitalWrite(ESP_RST_Pin, LOW),
        pinMode(ESP_DOWN_Pin, OUTPUT),
        digitalWrite(ESP_DOWN_Pin, HIGH),
        vTaskDelay(100),
        digitalWrite(ESP_RST_Pin, HIGH)
    ));
    
    for(;;)
    {
        ESP_Dispose();
        Comm_Response();
        vTaskDelay(2);
    }
    
	/*
	if(Serial2.available())
    {
        String receivedString = Serial2.readString();
        Serial.printf("ESP32 received:%s\r\n", receivedString.c_str());
    }
	Serial2.println("AT");
    */
}







