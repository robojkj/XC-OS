/*
 * MIT License
 * Copyright (c) 2020 robojkj
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "rtc.h"



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

void RTC_Time_Set(uint8_t h, uint8_t m, uint8_t s)
{
    RTC_TimeTypeDef RTC_TimeStructure;

    // 初始化时间
    RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
    RTC_TimeStructure.RTC_Hours = h;
    RTC_TimeStructure.RTC_Minutes = m;
    RTC_TimeStructure.RTC_Seconds = s;
    RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
    RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

void RTC_Date_Set(uint8_t w, uint8_t d, uint8_t m, uint8_t y)
{
    RTC_DateTypeDef RTC_DateStructure;

    // 初始化日期
    RTC_DateStructure.RTC_WeekDay = w;
    RTC_DateStructure.RTC_Date = d;
    RTC_DateStructure.RTC_Month = m;
    RTC_DateStructure.RTC_Year = y;
    RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
    RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
uint8_t RTC_CLK_Config(void)
{
    RTC_InitTypeDef RTC_InitStructure;

    /*使能 PWR 时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
    PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)
    /* 使用LSI作为RTC时钟源会有误差
     * 默认选择LSE作为RTC的时钟源
     */
    /* 使能LSI */
    RCC_LSICmd(ENABLE);
    /* 等待LSI稳定 */
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }
    /* 选择LSI做为RTC的时钟源 */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#elif defined (RTC_CLOCK_SOURCE_LSE)

    /* 使能LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* 等待LSE稳定 */
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }
    /* 选择LSE做为RTC的时钟源 */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

#endif /* RTC_CLOCK_SOURCE_LSI */

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
        return 0;
        //Serial.printf("\n\r RTC 时钟初始化失败 \r\n");
    }
    return 1;
}

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
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
#define LSE_STARTUP_TIMEOUT     ((uint16_t)0x05000)
uint8_t RTC_CLK_Config_Backup(void)
{
    __IO uint16_t StartUpCounter = 0;
    FlagStatus LSEStatus = RESET;
    RTC_InitTypeDef RTC_InitStructure;

    /* 使能 PWR 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
    PWR_BackupAccessCmd(ENABLE);

    /*=========================选择RTC时钟源==============================*/
    /* 默认使用LSE，如果LSE出故障则使用LSI */
    /* 使能LSE */
    RCC_LSEConfig(RCC_LSE_ON);

    /* 等待LSE启动稳定，如果超时则退出 */
    do
    {
        LSEStatus = RCC_GetFlagStatus(RCC_FLAG_LSERDY);
        StartUpCounter++;
    }
    while((LSEStatus == RESET) && (StartUpCounter != LSE_STARTUP_TIMEOUT));


    if(LSEStatus == SET )
    {
        //Serial.printf("\n\r LSE 启动成功 \r\n");
        /* 选择LSE作为RTC的时钟源 */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    }
    else
    {
        //Serial.printf("\n\r LSE 故障，转为使用LSI \r\n");

        /* 使能LSI */
        RCC_LSICmd(ENABLE);
        /* 等待LSI稳定 */
        while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
        {
        }

        //Serial.printf("\n\r LSI 启动成功 \r\n");
        /* 选择LSI作为RTC的时钟源 */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    }

    /* 使能 RTC 时钟 */
    RCC_RTCCLKCmd(ENABLE);
    /* 等待 RTC APB 寄存器同步 */
    RTC_WaitForSynchro();

    /*=====================初始化同步/异步预分频器的值======================*/
    /* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */

    /* 设置异步预分频器的值为127 */
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    /* 设置同步预分频器的值为255 */
    RTC_InitStructure.RTC_SynchPrediv = 0xFF;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    /* 用RTC_InitStructure的内容初始化RTC寄存器 */
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
        //Serial.printf("\n\r RTC 时钟初始化失败 \r\n");
        return 0;
    }
    return 1;
}


uint8_t init_rtc(void)
{
    uint8_t info = 0;
    /*
     * 当我们配置过RTC时间之后就往备份寄存器0写入一个数据做标记
     * 所以每次程序重新运行的时候就通过检测备份寄存器0的值来判断
     * RTC 是否已经配置过，如果配置过那就继续运行，如果没有配置过
     * 就初始化RTC，配置RTC的时间。
     */

    /* RTC配置：选择时钟源，设置RTC_CLK的分频系数 */
    RTC_CLK_Config();

    //闹钟增加函数
    /* 闹钟设置 */
    //RTC_AlarmSet();

    if (RTC_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA)
    {
        /* 设置时间和日期 */
        RTC_TimeAndDate_Set();
        info = 1;
    }
    else
    {
        /* 检查是否电源复位 */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            //Serial.printf("\r\n 发生电源复位....\r\n");
            info = 2;
        }
        /* 检查是否外部复位 */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            //Serial.printf("\r\n 发生外部复位....\r\n");
            info = 3;
        }

        //Serial.printf("\r\n 不需要重新配置RTC....\r\n");

        /* 使能 PWR 时钟 */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
        /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
        PWR_BackupAccessCmd(ENABLE);
        /* 等待 RTC APB 寄存器同步 */
        RTC_WaitForSynchro();

        //闹钟增加函数
        /* 清除RTC中断标志位 */
        //RTC_ClearFlag(RTC_FLAG_ALRAF);
        /* 清除 EXTI Line 17 悬起位 (内部连接到RTC Alarm) */
        //EXTI_ClearITPendingBit(EXTI_Line17);
    }
    return info;
}


// 闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
    {
        RTC_ClearITPendingBit(RTC_IT_ALRA);
        EXTI_ClearITPendingBit(EXTI_Line17);
    }
    /* 闹钟时间到，蜂鸣器响 */
    //BEEP_ON;
}





