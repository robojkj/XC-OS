#ifndef __BSP_H
#define __BSP_H

#include "Arduino.h"

//#include "BSP_MemoryPool.h"
#include "tinyalloc/tinyalloc.h"

/*Motor*/
void Motor_SetEnable(bool en);
void Motor_Vibrate(float strength, uint32_t time);
void Motor_SetState(bool state);

/*Power*/
void Power_Shutdown();
void Power_GetInfo(float* battCurrent,float* battVoltage,float* battVoltageOc);
float Power_GetBattUsage();

/*Audio*/
void Audio_SetEnable(bool en);
void Audio_WriteData(uint16_t data, uint16_t data2);
void Audio_ToneSetVolume(uint16_t volume);
uint16_t Audio_ToneGetVolume();
void Audio_NoTone();
void Audio_Tone(float freq);
void Audio_Tone(float freq, uint32_t time);
/*
uint32_t MemPool_GetResidueSize();
void MemPool_Clear(uint8_t n = 0);
void MemPool_ALLFree();
*/
/*Backlight*/
uint16_t Backlight_GetValue();
void Backlight_SetValue(int16_t val);
void Backlight_SetGradual(uint16_t target, uint16_t time = 500);


typedef void(*DAC_CallbackFunction_t)(void);

void dac_tim_dma_init(uint8_t DACx, TIM_TypeDef* TIMx, uint32_t freq, const void *buffer1, const void *buffer2, uint16_t num, DAC_CallbackFunction_t function);
void dma_Start(DMA_Stream_TypeDef* DMA_Streamx, uint32_t DMA_PAR, const void *buffer, uint16_t size);





#endif


