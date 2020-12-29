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
#ifndef __FSMC_H
#define __FSMC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_type.h"


//硬件相关的子函数
#define LCD_ADDR_DATA    ((uint32_t)0x60020000)	//Disp Data ADDR
#define LCD_ADDR_BASE    ((uint32_t)0x60000000)	//Disp Reg ADDR

void FSMC_Init(void);
void FSMC_WriteCommond(uint16_t CMD);
void FSMC_WriteData(uint16_t tem_data);
	
	
#ifdef __cplusplus
}
#endif

#endif
