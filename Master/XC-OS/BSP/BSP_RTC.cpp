#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"
#include "rtc.h"

RTC_TimeTypeDef RTC_TimeStructure;
RTC_DateTypeDef RTC_DateStructure;

uint8_t Rtctmp = 0;
void Task_RTC_Update(TimerHandle_t xTimer)
{
    __ExecuteOnce(
        init_rtc()
    );

    // 获取日历
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

    // 每秒打印一次
    if(Rtctmp != RTC_TimeStructure.RTC_Seconds)
    {
		// 打印日期
        Serial.printf("The Date :  Y:20%0.2d - M:%0.2d - D:%0.2d - W:%0.2d\r\n",
                      RTC_DateStructure.RTC_Year,
                      RTC_DateStructure.RTC_Month,
                      RTC_DateStructure.RTC_Date,
                      RTC_DateStructure.RTC_WeekDay);

        // 打印时间
        Serial.printf("The Time :  %0.2d:%0.2d:%0.2d \r\n\r\n",
                      RTC_TimeStructure.RTC_Hours,
                      RTC_TimeStructure.RTC_Minutes,
                      RTC_TimeStructure.RTC_Seconds);
        (void)RTC->DR;
    }
    Rtctmp = RTC_TimeStructure.RTC_Seconds;
}








