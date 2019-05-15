#include "sram.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//外部SRAM 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/14
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 


//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18

#define Bank1_SRAM3_ADDR		(u32)(0x68000000)

//初始化外部SRAM
void FSMC_SRAM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef FSMC_ReadWriteTimingStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG,ENABLE); //使能PD,PE,PF,PG时钟
	
	GPIO_InitStructure.GPIO_Pin =( GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10 \
																|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); //PD0,1,4,5,8,9,10,11,12,13,14,15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100M时钟
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推完模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11 \
																|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); //PE0,1,7,8,9,10,11,12,13,14,15
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_12 \
																|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15); //PF0,1,2,3,4,5,12,13,14,15
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_10 ; //PG0,1,2,3,4,5,10
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	//GPIOD复用配置
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);  //PD0
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);  //PD1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);  //PD4
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);  //PD5
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);  //PD8
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);  //PD9
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC); //PD10
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC); //PD11
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_FSMC); //PD12
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC); //PD13
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC); //PD14
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC); //PD15
	
	//GPIOE复用配置
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_FSMC);  //PE0
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_FSMC);  //PE1
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);  //PE7
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);  //PE8
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);  //PE9
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC); //PE10
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC); //PE11
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC); //PE12
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC); //PE13
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC); //PE14
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC); //PE15
	
	//GPIOF复用配置
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource0,GPIO_AF_FSMC);  //PF0
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource1,GPIO_AF_FSMC);  //PF1
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource2,GPIO_AF_FSMC);  //PF2
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource3,GPIO_AF_FSMC);  //PF3
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource4,GPIO_AF_FSMC);  //PF4
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource5,GPIO_AF_FSMC);  //PF5
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC); //PF12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource13,GPIO_AF_FSMC); //PF13
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource14,GPIO_AF_FSMC); //PF14
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource15,GPIO_AF_FSMC); //PF15
	
	//GPIOG复用配置
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource0,GPIO_AF_FSMC);  //PG0
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource1,GPIO_AF_FSMC);  //PG1
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource2,GPIO_AF_FSMC);  //PG2
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource3,GPIO_AF_FSMC);  //PG3
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource4,GPIO_AF_FSMC);  //PG4
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource5,GPIO_AF_FSMC);  //PG5
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource10,GPIO_AF_FSMC); //PG10
	
	FSMC_ReadWriteTimingStructure.FSMC_AddressSetupTime = 0X00; //地址建立时间0个HCLK 0ns
	FSMC_ReadWriteTimingStructure.FSMC_AddressHoldTime = 0x00;  //地址保持时间,模式A未用到
	FSMC_ReadWriteTimingStructure.FSMC_DataSetupTime = 0x09;    //数据保持时间,9个HCLK，6*9=54ns
	FSMC_ReadWriteTimingStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_ReadWriteTimingStructure.FSMC_CLKDivision = 0x00;
	FSMC_ReadWriteTimingStructure.FSMC_DataLatency = 0x00;
	FSMC_ReadWriteTimingStructure.FSMC_AccessMode = FSMC_AccessMode_A; //模式A
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;  //NOR/SRAM的Bank3
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; //数据和地址线不复用
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM; //SRAM存储模式
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b; //16位数据宽度
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable; //FLASH使用的,SRAM未使用
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable; //是否使能同步传输模式下的等待信号,此处未用到
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low; //等待信号的极性,仅在突发模式访问下有用
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;  //是否使能环路突发模式,此处未用到
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; //存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable; //存储器写使能
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   //等待使能位,此处未用到
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; //读写使用相同的时序
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  //异步传输期间的等待信号
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_ReadWriteTimingStructure;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_ReadWriteTimingStructure;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //FSMC_SRAM初始化
	
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3,ENABLE);  //使能NOR/SRAM功能
}

//在指定地址(WriteAddr+Bank1_SRAM3_ADDR)开始,连续写入n个字节.
//pBuffer:字节指针
//WriteAddr:要写入的地址
//n:要写入的字节数
void FSMC_SRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*(vu8*)(Bank1_SRAM3_ADDR + WriteAddr) = *pBuffer; 
		WriteAddr++;
		pBuffer++;
	}
}

//在指定地址((WriteAddr+Bank1_SRAM3_ADDR))开始,连续读出n个字节.
//pBuffer:字节指针
//ReadAddr:要读出的起始地址
//n:要写入的字节数
void FSMC_SRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*pBuffer ++=*(vu8*)(Bank1_SRAM3_ADDR + ReadAddr);
		ReadAddr++;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//测试函数
//在指定地址写入1个字节
//addr:地址
//data:要写入的数据
void fsmc_sram_test_write(u32 addr,u8 data)
{
	FSMC_SRAM_WriteBuffer(&data,addr,1);//写入一个字节
}

//读取1个字节
//addr:要读取的地址
//返回值:读取到的数据
u8 fsmc_sram_test_read(u32 addr)
{
	u8 data;
	FSMC_SRAM_ReadBuffer(&data,addr,1);
	return data;
}

