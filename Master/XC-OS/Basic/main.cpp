#include "FileGroup.h"
#include "TasksManage.h"

static void setup()
{
    Serial.begin(460800);
    Serial.println("\r\n+===================== XC-OS =======================+\r\n");
    Serial.println("@FreeRTOS Kernel "tskKERNEL_VERSION_NUMBER);
    Serial.print(XC_OS_NAME"("__DATE__" "__TIME__" Build) init...\r\n");
    Wire.begin();

    /*Task Create*/
    xTaskReg(Display,      KByteToWord(6),    2);
    xTaskReg(PageRun,      KByteToWord(4),    1);
    xTaskReg(WavPlayer,    KByteToWord(3),    2);
    //xTaskReg(Communicate,  KByteToWord(1),    0);
#if( XC_USE_LUA == 1 )
    xTaskReg(LuaScript,    KByteToWord(5),    0);
#endif

    /*Timer Create*/
    xTimerReg(Task_BattInfoUpdate, 500);
    xTimerReg(Task_MotorRunning, 10);
    //xTimerReg(Task_IMU_Process, 20);
    xTimerReg(Task_SensorInfoUpdate, 1000);
	xTimerReg(Task_WirelessComm, 1000);
    //xTimerReg(Task_RTC_Update, 20);

    Serial.println("OS start scheduler...");
    Serial.print("\r\n+===================================================+\r\n");
    Delay_Init();
    vTaskStartScheduler();
}

static void loop()
{
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x10000);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    setup();
    for(;;)loop();
}

