#include "Basic/FileGroup.h"
#include "BSP.h"

#define USE_AUDIO_TONE 1

#define DAC_CH1_Pin PA4
#define DAC_CH2_Pin PA5

#define IS_DAC_Pin(Pin) (Pin==DAC_CH1_Pin||Pin==DAC_CH2_Pin)

static volatile uint32_t* Audio_RegisterBase = 0;
static volatile uint32_t* Audio_RegisterBase2 = 0;

static void Audio_GetChannel1_RegisterBase(uint32_t DAC_Align, uint32_t DAC_Align2)
{
	#define DHR12R1_OFFSET ((uint32_t)0x00000008)
	#define DHR12R2_OFFSET ((uint32_t)0x00000014)
    __IO uint32_t tmp = 0;

    /* Check the parameters */
    assert_param(IS_DAC_ALIGN(DAC_Align));
	assert_param(IS_DAC_ALIGN(DAC_Align2));

    tmp = (uint32_t)DAC_BASE;
    tmp += DHR12R1_OFFSET + DAC_Align;
    Audio_RegisterBase = (volatile uint32_t*)tmp;
	
	tmp = (uint32_t)DAC_BASE;
    tmp += DHR12R2_OFFSET + DAC_Align2;
    Audio_RegisterBase2 = (volatile uint32_t*)tmp;
}

static void Audio_Init()
{
    pinMode(Audio_MUTE_Pin, OUTPUT);
	/*
    if(!IS_DAC_Pin(SpeakerA_Pin) && !IS_DAC_Pin(SpeakerB_Pin))
        return;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    DAC_InitTypeDef dacConfig;
    DAC_StructInit(&dacConfig);
    dacConfig.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    dacConfig.DAC_Trigger = DAC_Trigger_None;
    dacConfig.DAC_WaveGeneration = DAC_WaveGeneration_None;
	
	DAC_Init(DAC_Channel_1, &dacConfig);
	DAC_Cmd(DAC_Channel_1, ENABLE);
	
	DAC_Init(DAC_Channel_2, &dacConfig);
	DAC_Cmd(DAC_Channel_2, ENABLE);
	
	Audio_GetChannel1_RegisterBase(DAC_Align_12b_R, DAC_Align_12b_R);
	
    pinMode(SpeakerA_Pin, OUTPUT_AF);
	pinMode(SpeakerB_Pin, OUTPUT_AF);
	*/
}

void Audio_SetEnable(bool en)
{
    __ExecuteOnce(Audio_Init());

    digitalWrite(Audio_MUTE_Pin, en);
}

void Audio_WriteData(uint16_t data, uint16_t data2)
{
    *Audio_RegisterBase = data;
	*Audio_RegisterBase2 = data2;
}



static DAC_CallbackFunction_t DACx_Function[2] = {0};


void dac_tim_dma_init(uint8_t DACx, TIM_TypeDef* TIMx, uint32_t freq, const void *buffer1, const void *buffer2, uint16_t num, DAC_CallbackFunction_t function)
{
	DAC_InitTypeDef DAC_InitStructure;
	DMA_Stream_TypeDef* DMA_Streamx;
	DMA_InitTypeDef DMA_InitStructure;
	uint32_t DAC_CHx;
	uint8_t DMA_IRQx;
	uint32_t DAC_TRGOx;
	uint32_t DMA_PAR;
	
	if(TIMx != TIM2 && TIMx != TIM4 && TIMx != TIM5 && 
		TIMx != TIM6 && TIMx != TIM7 && TIMx != TIM8)
		return;
	
	if(TIMx == TIM2)	DAC_TRGOx = DAC_Trigger_T2_TRGO;
	if(TIMx == TIM4)	DAC_TRGOx = DAC_Trigger_T4_TRGO;
	if(TIMx == TIM5)	DAC_TRGOx = DAC_Trigger_T5_TRGO;
	if(TIMx == TIM6)	DAC_TRGOx = DAC_Trigger_T6_TRGO;
	if(TIMx == TIM7)	DAC_TRGOx = DAC_Trigger_T7_TRGO;
	if(TIMx == TIM8)	DAC_TRGOx = DAC_Trigger_T8_TRGO;
	
	if(DACx == 1)
	{
		pinMode(PA4, INPUT_ANALOG);
		DAC_CHx = DAC_Channel_1;
		DMA_Streamx = DMA1_Stream5;
		DMA_IRQx = DMA1_Stream5_IRQn;
		DMA_PAR = (uint32)&DAC->DHR12R1;
	}else if(DACx == 2)
	{
		pinMode(PA5, INPUT_ANALOG);
		DAC_CHx = DAC_Channel_2;
		DMA_Streamx = DMA1_Stream6;
		DMA_IRQx = DMA1_Stream6_IRQn;
		DMA_PAR = (uint32)&DAC->DHR12R2;
	}else
	{
		return;
	}
	
	DACx_Function[DACx-1] = function;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	
	Timer_SetTrigger(TIMx, freq);

	DMA_DeInit(DMA_Streamx);
	DMA_InitStructure.DMA_Channel = DMA_Channel_7;
	DMA_InitStructure.DMA_PeripheralBaseAddr = DMA_PAR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32)buffer1;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = num;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	//DMA_Mode_Circular DMA_Mode_Normal
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA_Streamx, &DMA_InitStructure);
	
	if(buffer2 != NULL)
	{
		DMA_DoubleBufferModeConfig(DMA_Streamx,(uint32_t)buffer1, DMA_Memory_0);//双缓冲模式配置
		DMA_DoubleBufferModeConfig(DMA_Streamx,(uint32_t)buffer2, DMA_Memory_1);//双缓冲模式配置
		DMA_DoubleBufferModeCmd(DMA_Streamx, ENABLE);//双缓冲模式开启
	}
	
	//NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	NVIC_InitTypeDef NVIC_InitStructure; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQx;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_ITConfig(DMA_Streamx, DMA_IT_TC, ENABLE);  //配置DMA发送完成后产生中断
	DMA_Cmd(DMA_Streamx, ENABLE);

	DAC_StructInit(&DAC_InitStructure);
	DAC_InitStructure.DAC_Trigger = DAC_TRGOx;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_CHx,&DAC_InitStructure);

	DAC_Cmd(DAC_CHx, ENABLE);
	DAC_DMACmd(DAC_CHx, ENABLE);
	
	
}

void dma_Start(DMA_Stream_TypeDef* DMA_Streamx, uint32_t DMA_PAR, const void *buffer, uint16_t size)
{
	DMA_Cmd(DMA_Streamx, DISABLE);                      //关闭DMA传输
	DMA_Streamx->PAR = DMA_PAR;
    DMA_Streamx->M0AR = (uint32)buffer;
    DMA_Streamx->NDTR = size;
    DMA_Cmd(DMA_Streamx, ENABLE);                      //开启DMA传输
}

extern "C" {
void DMA1_Stream5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5))
    {
		//Serial.println("ok1....");
		DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5); //清除全部中断标志
		
		if(DACx_Function[0]) DACx_Function[0]();
    }
}
void DMA1_Stream6_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6))
    {
		//Serial.println("ok2....");
		DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6); //清除全部中断标志
		
		if(DACx_Function[1]) DACx_Function[1]();
    }
}
}


#include "arm_math.h"

#define AUDIO_TONE_SAMPLE_FREQ   50000
#define AUDIO_TONE_HZ_TO_US(hz) (1000000.0f/(hz))
#define AUDIO_TONE_TIM           TIM1
#define AUDIO_TONE_FREQ_SLIDE_TICKS  50

static const uint32_t Tone_SampleTime = AUDIO_TONE_HZ_TO_US(AUDIO_TONE_SAMPLE_FREQ);
static uint32_t Tone_SampleTick;
static uint32_t Tone_StopTime;
static bool Tone_AutoStop = false;
static float Tone_SignalPeriod;
static float Tone_SignalFreq = 0;
static float Tone_CurrentSignalFreq = 0;
static float Tone_LastSignalFreq = 0;
static float Tone_TargetSignalFreq = 0;
static uint16_t Tone_Volume;

void Audio_ToneSetVolume(uint16_t volume)
{
    Tone_Volume = volume;
}

uint16_t Audio_ToneGetVolume()
{
    return Tone_Volume;
}

void Audio_NoTone()
{
    TIM_Cmd(AUDIO_TONE_TIM, DISABLE);
    Tone_CurrentSignalFreq = Tone_LastSignalFreq = Tone_SignalFreq = 0;
}
void Audio_FreqSlider()
{
    static uint32_t freqSliderTick = 0;
    if(Tone_SignalFreq != Tone_TargetSignalFreq)
    {
        freqSliderTick = 0;
        Tone_LastSignalFreq = Tone_CurrentSignalFreq;
        Tone_SignalFreq = Tone_TargetSignalFreq;
    }
    if(fabs(Tone_LastSignalFreq) <= 1)
    {
        Tone_CurrentSignalFreq = Tone_SignalFreq;
    }
    else
    {
        Tone_CurrentSignalFreq = fmap(
        freqSliderTick, 0, AUDIO_TONE_FREQ_SLIDE_TICKS,
        Tone_LastSignalFreq, Tone_SignalFreq
        );
    }
    if(freqSliderTick < AUDIO_TONE_FREQ_SLIDE_TICKS)
    {
        freqSliderTick++;
    }
    Tone_SignalPeriod = AUDIO_TONE_HZ_TO_US(Tone_CurrentSignalFreq);
}

static void Audio_ToneTimerCallback()
{
    if(Tone_AutoStop && millis() >= Tone_StopTime)
    {
        Audio_NoTone();
        return;
    }
    Audio_FreqSlider();
    Tone_SampleTick += 0x10000 * Tone_SampleTime / Tone_SignalPeriod;
    float val = (float)Tone_SampleTick / 0x10000;
    val -= (int)val;
    uint16_t output = arm_sin_q15(val * 32767) + 32768;
	uint32_t data = output * ((float)Tone_Volume / 0xFFFF);
    Audio_WriteData(data, data);
    Tone_SampleTick++;
}

static void Audio_ToneInit()
{
    Timer_SetInterrupt(AUDIO_TONE_TIM, 0xFF, Audio_ToneTimerCallback);
    Timer_SetInterruptFreqUpdate(AUDIO_TONE_TIM, AUDIO_TONE_SAMPLE_FREQ);
}
void Audio_Tone(float freq)
{
    __ExecuteOnce(Audio_ToneInit());
    
    if(freq <= 1 || freq > 20000)
    {
        Audio_NoTone();
        return;
    }
    //TIM_Cmd(AUDIO_TONE_TIM, DISABLE);
    Tone_AutoStop = false;
    Tone_TargetSignalFreq = freq;
    TIM_Cmd(AUDIO_TONE_TIM, ENABLE);
}

void Audio_Tone(float freq, uint32_t time)
{
    if(freq <= 1 || freq > 20000 || time == 0)
    {
        Audio_NoTone();
        return;
    }
    Audio_Tone(freq);
    Tone_AutoStop = true;
    Tone_StopTime = millis() + time;
}


//#if (USE_AUDIO_TONE == 1)

//#include "arm_math.h"

//#define AUDIO_TONE_SAMPLE_FREQ   50000
//#define AUDIO_TONE_HZ_TO_US(hz) (1000000.0f/(hz))
//#define AUDIO_TONE_TIM           TIM1

//static const uint32_t Tone_SamplePeriod = AUDIO_TONE_HZ_TO_US(AUDIO_TONE_SAMPLE_FREQ);
//static uint32_t Tone_SampleTick;
//static uint32_t Tone_StopTime;
//static bool Tone_AutoStop = false;
//static float Tone_SignalPeriod;
//static uint16_t Tone_Volume;

//void Audio_ToneSetVolume(uint16_t volume)
//{
//    Tone_Volume = volume;
//}

//uint16_t Audio_ToneGetVolume()
//{
//    return Tone_Volume;
//}

//void Audio_NoTone()
//{
//    TIM_Cmd(AUDIO_TONE_TIM, DISABLE);
//}

//static void Audio_ToneTimerCallback()
//{
//    if(Tone_AutoStop && millis() >= Tone_StopTime)
//    {
//        Audio_NoTone();
//        return;
//    }

//    float fraction = (Tone_SampleTick * Tone_SamplePeriod) / Tone_SignalPeriod;
//    fraction -= (int)fraction;
//    uint16_t output = arm_sin_q15(fraction * 32767) + 32768;
//    Audio_WriteData(output * ((float)Tone_Volume / 0xFFFF));
//    Tone_SampleTick++;
//}

//static void Audio_ToneInit()
//{
//    Timer_SetInterrupt(AUDIO_TONE_TIM, 0xFF, Audio_ToneTimerCallback);
//    Timer_SetInterruptFreqUpdate(AUDIO_TONE_TIM, AUDIO_TONE_SAMPLE_FREQ);
//}

//void Audio_Tone(float freq)
//{
//    __ExecuteOnce(Audio_ToneInit());
//    
//    if(freq <= 1 || freq > 20000)
//    {
//        Audio_NoTone();
//        return;
//    }
//    TIM_Cmd(AUDIO_TONE_TIM, DISABLE);
//    Tone_AutoStop = false;
//    Tone_SampleTick = 0;
//    Tone_SignalPeriod = AUDIO_TONE_HZ_TO_US(freq);
//    TIM_Cmd(AUDIO_TONE_TIM, ENABLE);
//}

//void Audio_Tone(float freq, uint32_t time)
//{
//    if(freq <= 1 || freq > 20000 || time == 0)
//    {
//        Audio_NoTone();
//        return;
//    }
//    Audio_Tone(freq);
//    Tone_AutoStop = true;
//    Tone_StopTime = millis() + time;
//}

//#endif
