#ifndef __ESP_COMM_H
#define __ESP_COMM_H

#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"

void change_uart_baud(uint8_t id, u32 baud);
void esp_tool_protocol(uint8_t inD);
void esp_update_protocol(uint8_t inD);
void ESP_Dispose();

#endif
