# XC-OS
> 面向MCU的小型嵌入式图形操作系统

>[硬件设计](https://github.com/robojkj/XC-OS-Hardware)

# 原仓库地址
> https://github.com/FASTSHIFT/XC-OS

# V3.01更新内容 2020/12/13 
* 1.同步硬件V2.1更新引脚定义

## 基于XC-OSV2.6
* 1.屏幕显示-FSMC/DMA-借鉴[WatchX](https://github.com/FASTSHIFT/WatchX)手表项目,采用乒乓缓冲方式刷新图像
* 2.文件系统-SDIO/DMA-适配FATFS
* 3.音乐播放器-增加MP3格式，实现进度条拖动功能，优化快进停止播放等按钮卡死问题。
* 4.音频输出-底层改用DAC/TIM/DMA-双缓冲方式播放16位格式PCM
* 5.电源管理-修复协议相关BUG
* 6.USB升级功能-采用STM32硬件USB,搭配上位机更新程序

## 硬件配置
* 主控: STM32F429VGT6(RAM:256KB ROM:1024KB FREQ:180MHz)
* 屏幕: LCD R61529(3.5inch 480x320 16bit总线)
* 触控: FT5436
* 功放: PAM8403(3W)
* 扬声器: OPPOR17 扬声器总成
* 振动器: OPPO a37
* 电池: Li-ion 3.7v 2000maH
* 电源管理: IP5108(5V 2A)
* 加速度传感器: MPU6050
* 外置储存器: Micro SD
* 通信: NRF24L01+PA
* USB接口:CH340E串口+STM32内部USB

## USB引导更新程序
![image](https://github.com/robojkj/XC-OS/blob/master/Usb_UpdateAPP/1.jpg)

## 成品展示
![image](https://github.com/robojkj/XC-OS/blob/master/Images/1.jpg)
![image](https://github.com/robojkj/XC-OS/blob/master/Images/2.jpg)
![image](https://github.com/robojkj/XC-OS/blob/master/Images/3.jpg)
![image](https://github.com/robojkj/XC-OS/blob/master/Images/4.jpg)
![image](https://github.com/robojkj/XC-OS/blob/master/Images/5.jpg)


