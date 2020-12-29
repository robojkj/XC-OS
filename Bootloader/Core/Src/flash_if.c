/*
*******************************************************************************************************
*
* 文件名称 : flash_if.c
* 版    本 : V1.0
* 作    者 : OpenRabbit
* 说    明 : STM32内部FLASH驱动文件
* 
*******************************************************************************************************
*/



/* 头文件 -----------------------------------------------------------*/
#include "flash_if.h"


/* 宏定义 -----------------------------------------------------------*/


/* 变量 -------------------------------------------------------------*/


/* 函数声明 ---------------------------------------------------------*/




/**
  * @brief 获取某个地址所在的flash扇区
  * @param addr-flash地址
  * @retval	0~11-addr所在的扇区
  * @note 扇区大小不一致
  */
uint8_t flash_if_get_sector(uint32_t addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_SECTOR_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_SECTOR_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_SECTOR_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_SECTOR_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_SECTOR_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_SECTOR_10; 
	return FLASH_SECTOR_11;	
}



/**
  * @brief 按字读FLASH数据
  * @param addr-读地址
  * @retval	读取到的数据
  */
uint32_t flash_if_read_word(uint32_t faddr)
{
	return *(__IO uint32_t *)faddr; 
}






/**
  * @brief 从指定地址开始写入指定长度的数据
  * @param pbuffer-数据缓冲区
  * @param write_addr-写地址(必须为4的倍数)
  * @param num_byte_to_write-要写的数据个数
  * @retval	None
  * @note 必须按字写,即每次写32位数据
  * @note 该函数带擦除操作,但并不会保留扇区数据
  */
void flash_if_write(uint32_t *pbuffer,uint32_t write_addr,uint32_t num_byte_to_write)	
{ 
    FLASH_EraseInitTypeDef FlashEraseInit;
    HAL_StatusTypeDef FlashStatus=HAL_OK;
    uint32_t SectorError=0;
	uint32_t addrx=0;
	uint32_t endaddr=0;	
    if(write_addr<STM32_FLASH_BASE||write_addr%4)return;	//非法地址
    
 	HAL_FLASH_Unlock();             //解锁	
	addrx=write_addr;				//写入的起始地址
	endaddr=write_addr+num_byte_to_write*4;	//写入的结束地址
    
    if(addrx<0X1FFF0000)
    {
        while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if(flash_if_read_word(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{   
                FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除 
                FlashEraseInit.Sector=flash_if_get_sector(addrx);   //要擦除的扇区
                FlashEraseInit.NbSectors=1;                             //一次只擦除一个扇区
                FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
                if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
                    break;//发生错误了	
			}
			else
				addrx+=4;
            FLASH_WaitForLastOperation(FLASH_WAITETIME);                //等待上次操作完成
        }
    }
    FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);            //等待上次操作完成
	if(FlashStatus==HAL_OK)
	{
		while(write_addr<endaddr)//写数据
		{
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,write_addr,*pbuffer)!=HAL_OK)//写入数据
			{ 
				break;	//写入异常
			}
			write_addr+=4;
			pbuffer++;

		} 
	}
	HAL_FLASH_Lock();           //上锁
} 












/***************************** 跳兔科技 www.whtiaotu.com (END OF FILE) *********************************/
