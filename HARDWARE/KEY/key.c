#include "key.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//按键输入驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//按键初始化函数
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE,ENABLE);  //使能GPIOA GPIOE时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  //PE2,3,4引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;  //输入
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //上拉输入
	GPIO_Init(GPIOE,&GPIO_InitStructure); 			//初始化GPIOE
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;  //PA0引脚
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  //下拉输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);     //初始化GPIOA
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{
	static u8 key_up=1;   //按键松开标志
	if(mode) key_up=1;  //支持连按
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1)) //有按键按下
	{
		delay_ms(10);  //按键去抖
		key_up=0;
		if(KEY0==0) return 1;
		else if(KEY1==0) return 2;
		else if(KEY2==0) return 3;
		else if(WK_UP==1) return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1; 
	return 0; //无按键按下
}

