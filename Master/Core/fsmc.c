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
#include "fsmc.h"


void FSMC_Init(void)
{
	FMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FMC_NORSRAMTimingInitTypeDef  p; 
    
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC,	ENABLE);
	
	FMC_NORSRAMStructInit(&FSMC_NORSRAMInitStructure);
	
    p.FMC_AddressSetupTime = 0x02;	 //地址建立时间
    p.FMC_AddressHoldTime = 0x00;	 //地址保持时间
    p.FMC_DataSetupTime = 0x03;		 //数据建立时间
    p.FMC_BusTurnAroundDuration = 0x00;
    p.FMC_CLKDivision = 0x00;
    p.FMC_DataLatency = 0x00;

    p.FMC_AccessMode = FMC_AccessMode_B;	 // 一般使用模式B来控制LCD
    
    FSMC_NORSRAMInitStructure.FMC_Bank = FMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FMC_DataAddressMux = FMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FMC_MemoryType = FMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FMC_MemoryDataWidth = FMC_NORSRAM_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FMC_BurstAccessMode = FMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FMC_AsynchronousWait = FMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FMC_WrapMode = FMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FMC_WriteOperation = FMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FMC_WaitSignal = FMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FMC_ExtendedMode = FMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FMC_WriteBurst = FMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FMC_WriteTimingStruct = &p; 
    
    FMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* Enable FSMC Bank1_SRAM Bank */
    FMC_NORSRAMCmd(FMC_Bank1_NORSRAM1, ENABLE);  
}

 void FSMC_WriteCommond(uint16_t CMD)
{			
	*(__IO uint16_t *) (LCD_ADDR_BASE) = CMD;
}
 void FSMC_WriteData(uint16_t tem_data)
{			
	*(__IO uint16_t *) (LCD_ADDR_DATA) = tem_data;
}

















