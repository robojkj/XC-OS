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
#ifndef __RTC_H
#define __RTC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_type.h"


// 时钟源宏定义
#define RTC_CLOCK_SOURCE_LSE
//#define RTC_CLOCK_SOURCE_LSI

// 异步分频因子
#define ASYNCHPREDIV         0X7F
// 同步分频因子
#define SYNCHPREDIV          0XFF



// 时间宏定义
#define RTC_H12_AMorPM           RTC_H12_AM
#define HOURS                1          // 0~23
#define MINUTES              1          // 0~59
#define SECONDS              1          // 0~59

// 日期宏定义
#define WEEKDAY              1         // 1~7
#define DATE                 1         // 1~31
#define MONTH                1         // 1~12
#define YEAR                 1         // 0~99


// 闹钟相关宏定义
#define ALARM_HOURS               1          // 0~23
#define ALARM_MINUTES             5          // 0~59
#define ALARM_SECONDS             15          // 0~59


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


uint8_t RTC_CLK_Config(void);
void RTC_TimeAndDate_Set(void);
void RTC_Time_Set(uint8_t h, uint8_t m, uint8_t s);
void RTC_Date_Set(uint8_t w, uint8_t d, uint8_t m, uint8_t y);
void RTC_AlarmSet(void);

uint8_t init_rtc(void);



#ifdef __cplusplus
}
#endif

#endif
