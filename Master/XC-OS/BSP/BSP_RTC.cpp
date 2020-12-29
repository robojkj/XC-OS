#include "Basic/FileGroup.h"
#include "Basic/TasksManage.h"

// 异步分频因子
#define ASYNCHPREDIV         0X7F
// 同步分频因子
#define SYNCHPREDIV          0XFF

// 时间宏定义
#define RTC_H12_AMorPM                RTC_H12_AM
#define HOURS                     1          // 0~23
#define MINUTES                   1          // 0~59
#define SECONDS                   1          // 0~59


// 日期宏定义
#define WEEKDAY                   1         // 1~7
#define DATE                      1         // 1~31
#define MONTH                     1         // 1~12
#define YEAR                      1         // 0~99

// 闹钟相关宏定义
#define ALARM_HOURS               1          // 0~23
#define ALARM_MINUTES             1          // 0~59
#define ALARM_SECONDS             5          // 0~59

#define ALARM_MASK                RTC_AlarmMask_DateWeekDay
#define ALARM_DATE_WEEKDAY_SEL    RTC_AlarmDateWeekDaySel_WeekDay
#define ALARM_DATE_WEEKDAY        2
#define RTC_Alarm_X               RTC_Alarm_A

// 时间格式宏定义
#define RTC_Format_BINorBCD  RTC_Format_BIN

// 备份域寄存器宏定义
#define RTC_BKP_DRX          RTC_BKP_DR0
// 写入到备份寄存器的数据宏定义
#define RTC_BKP_DATA         0X32F2


/*
 *    要使能 RTC 闹钟中断，需按照以下顺序操作：
 * 1. 将 EXTI 线 17 配置为中断模式并将其使能，然后选择上升沿有效。
 * 2. 配置 NVIC 中的 RTC_Alarm IRQ 通道并将其使能。
 * 3. 配置 RTC 以生成 RTC 闹钟（闹钟 A 或闹钟 B）。
 *
 *
*/
void RTC_AlarmSet(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;
    RTC_AlarmTypeDef  RTC_AlarmStructure;

    /*=============================第①步=============================*/
    /* RTC 闹钟中断配置 */
    /* EXTI 配置 */
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /*=============================第②步=============================*/
    /* 使能RTC闹钟中断 */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*=============================第③步=============================*/
    /* 失能闹钟 ，在设置闹钟时间的时候必须先失能闹钟*/
    RTC_AlarmCmd(RTC_Alarm_X, DISABLE);
    /* 设置闹钟时间 */
    RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AMorPM;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = ALARM_HOURS;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = ALARM_MINUTES;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = ALARM_SECONDS;
    RTC_AlarmStructure.RTC_AlarmMask = ALARM_MASK;
    RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = ALARM_DATE_WEEKDAY_SEL;
    RTC_AlarmStructure.RTC_AlarmDateWeekDay = ALARM_DATE_WEEKDAY;


    /* 配置RTC Alarm X（X=A或B） 寄存器 */
    RTC_SetAlarm(RTC_Format_BINorBCD, RTC_Alarm_X, &RTC_AlarmStructure);

    /* 使能 RTC Alarm X 中断 */
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);

    /* 使能闹钟 */
    RTC_AlarmCmd(RTC_Alarm_X, ENABLE);

    /* 清除闹钟中断标志位 */
    RTC_ClearFlag(RTC_FLAG_ALRAF);
    /* 清除 EXTI Line 17 悬起位 (内部连接到RTC Alarm) */
    EXTI_ClearITPendingBit(EXTI_Line17);
}


/**
 * @brief  设置时间和日期
 * @param  无
 * @retval 无
 */
void RTC_TimeAndDate_Set(void)
{
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;

    // 初始化时间
    RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
    RTC_TimeStructure.RTC_Hours = HOURS;
    RTC_TimeStructure.RTC_Minutes = MINUTES;
    RTC_TimeStructure.RTC_Seconds = SECONDS;
    RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
    RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);

    // 初始化日期
    RTC_DateStructure.RTC_WeekDay = WEEKDAY;
    RTC_DateStructure.RTC_Date = DATE;
    RTC_DateStructure.RTC_Month = MONTH;
    RTC_DateStructure.RTC_Year = YEAR;
    RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
    RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

extern "C"{
// 闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{
  if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
  {    
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    EXTI_ClearITPendingBit(EXTI_Line17);
  }
	/* 闹钟时间到，蜂鸣器响 */
}
}

/* RTC配置：选择时钟源，设置RTC_CLK的分频系数 */
void init_rtc(void)
{
    RTC_InitTypeDef RTC_InitStructure;

    /*使能 PWR 时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
    PWR_BackupAccessCmd(ENABLE);

    /* 使能LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* 等待LSE稳定 */
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

    /* 选择LSE做为RTC的时钟源 */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* 使能RTC时钟 */
    RCC_RTCCLKCmd(ENABLE);

    /* 等待 RTC APB 寄存器同步 */
    RTC_WaitForSynchro();

    /*=====================初始化同步/异步预分频器的值======================*/
    /* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */

    /* 设置异步预分频器的值 */
    RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;
    /* 设置同步预分频器的值 */
    RTC_InitStructure.RTC_SynchPrediv = SYNCHPREDIV;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    /* 用RTC_InitStructure的内容初始化RTC寄存器 */
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
        Serial.printf("\r\n RTC 时钟初始化失败\r\n");
    }


    if (RTC_ReadBackupRegister(RTC_BKP_DRX) != 0xf)//RTC_BKP_DATA
    {
        /* 闹钟设置 */
        RTC_AlarmSet();

        /* 设置时间和日期 */
        RTC_TimeAndDate_Set();
    }
    else
    {
        /* 检查是否电源复位 */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            Serial.printf("\r\n 发生电源复位....\r\n");
        }
        /* 检查是否外部复位 */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            Serial.printf("\r\n 发生外部复位....\r\n");
        }

        Serial.printf("\r\n 不需要重新配置RTC....\r\n");

        /* 使能 PWR 时钟 */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
        /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
        PWR_BackupAccessCmd(ENABLE);
        /* 等待 RTC APB 寄存器同步 */
        RTC_WaitForSynchro();

        /* 清除RTC中断标志位 */
        RTC_ClearFlag(RTC_FLAG_ALRAF);
        /* 清除 EXTI Line 17 悬起位 (内部连接到RTC Alarm) */
        EXTI_ClearITPendingBit(EXTI_Line17);
    }



}

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








