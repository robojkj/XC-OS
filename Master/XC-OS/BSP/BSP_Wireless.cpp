#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "BSP.h"

void Task_WirelessComm(TimerHandle_t xTimer)
{
	__ExecuteOnce((
		pinMode(ESP_RST_Pin, OUTPUT),
		digitalWrite(ESP_RST_Pin, LOW),
		pinMode(ESP_DOWN_Pin, OUTPUT),
		//启用ESP32通信
		digitalWrite(ESP_DOWN_Pin, HIGH),
		Serial2.begin(115200),
		//进入ESP32烧录模式，以上启动ESP32通信代码要屏蔽
		//digitalWrite(ESP_DOWN_Pin, LOW),
		//pinMode(PA2, INPUT),
		//pinMode(PA3, INPUT),
		delay(100),
		digitalWrite(ESP_RST_Pin, HIGH)
    ));
	
	if(Serial2.available())
    {
        String receivedString = Serial2.readString();
        Serial.printf("ESP32 received:%s\r\n", receivedString.c_str());
    }
	Serial2.print("AT\r\n");
}








