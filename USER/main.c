 /**********************************************************************************************************
** 文件名		:main.c
** 作者			:maxlicheng<licheng.chn@outlook.com>
** 作者github	:https://github.com/maxlicheng
** 作者博客		:https://www.maxlicheng.com/	
** 生成日期		:2018-08-08
** 描述			:项目主文件，删减功能请在这里修改，注意任务数量大小
************************************************************************************************************/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lwip_comm.h"
#include "LAN8720.h"
#include "timer.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h"
#include "lwip_comm.h"
#include "includes.h"
#include "lwipopts.h"

#include "cJSON.h"
#include "mqtt_app.h"

#include "text.h"	

#include "ds18b20.h"
#include "sdio_sdcard.h"    
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"

//ALIENTEK 探索者STM32F407开发板
//LWIP LWIP+UCOS操作系统移植实验
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司


//在LCD上显示地址信息任务
//任务优先级
#define DISPLAY_TASK_PRIO	8
//任务堆栈大小
#define DISPLAY_STK_SIZE	128
//任务堆栈
__align(8) static OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//任务函数
void display_task(void *pdata);


//LED任务
//任务优先级
#define LED_TASK_PRIO		9
//任务堆栈大小
#define LED_STK_SIZE		64
//任务堆栈
__align(8) static OS_STK	LED_TASK_STK[LED_STK_SIZE];
//任务函数
void led_task(void *pdata);  


//START任务
//任务优先级
#define START_TASK_PRIO		10
//任务堆栈大小
#define START_STK_SIZE		128
//任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata); 

#define mqtt_task_prio	11
#define mqtt_stk_size   1024
__align(8) static OS_STK mqtt_task_stk[mqtt_stk_size];
void mqtt_task(void *pdata);

#define dht11_task_prio	12
#define dht11_stk_size   128
__align(8) static OS_STK dht11_task_stk[dht11_stk_size];
void dht11_task(void *pdata);


//在LCD上显示地址信息
//mode:1 显示DHCP获取到的地址
//	  其他 显示静态地址
void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==2)
	{
		sprintf((char*)buf,"DHCP IP :%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW :%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,170,210,16,16,buf); 
	}
	else 
	{
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK :%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,170,210,16,16,buf); 
	}	
}

int main(void)
{
	delay_init(168);       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置
	uart_init(115200);    	//串口波特率设置
	LED_Init();  			//LED初始化
	KEY_Init();  			//按键初始化
	LCD_Init();  			//LCD初始化
	DS18B20_Init();			//温度传感器初始化
	W25QXX_Init();				//初始化W25Q128
	FSMC_SRAM_Init();		//SRAM初始化
	
	mymem_init(SRAMIN);  	//初始化内部内存池
	mymem_init(SRAMEX);  	//初始化外部内存池
	mymem_init(SRAMCCM); 	//初始化CCM内存池
	
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	font_init();
	
	POINT_COLOR = RED; 		//红色字体
	LCD_ShowString(30,30,200,20,16,"Explorer STM32F4");
	LCD_ShowString(30,50,200,20,16,"LWIP+UCOS Test");
	LCD_ShowString(30,70,200,20,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,20,16,"2014/9/1");
//	Show_Str(30,244,200,16,"对应汉字(16*16)为:",16,0);

	OSInit(); 					//UCOS初始化
	while(lwip_comm_init()) 	//lwip初始化
	{
		LCD_ShowString(30,110,200,20,16,"Lwip Init failed!"); 	//lwip初始化失败
		delay_ms(500);
		LCD_Fill(30,110,230,150,WHITE);
		delay_ms(500);
	}
	LCD_ShowString(30,110,200,20,16,"Lwip Init Success!"); 		//lwip初始化成功
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart(); //开启UCOS
}

//start任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//初始化统计任务
	OS_ENTER_CRITICAL();  	//关中断
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); //创建DHCP任务
	OSTimeDlyHMSM(0,0,2,500);
#endif
	
	OSTaskCreate(led_task,(void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);//创建LED任务
	OSTaskCreate(display_task,(void*)0,(OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1],DISPLAY_TASK_PRIO); //显示任务
	OSTaskCreate(mqtt_task,(void*)0,(OS_STK*)&mqtt_task_stk[mqtt_stk_size-1],mqtt_task_prio); //mqtt任务
	OSTaskCreate(dht11_task,(void*)0,(OS_STK*)&dht11_task_stk[dht11_stk_size-1],dht11_task_prio); //mqtt任务
	
	OSTaskSuspend(OS_PRIO_SELF); 	//挂起start_task任务
	OS_EXIT_CRITICAL();  			//开中断
}

//显示地址等信息
void display_task(void *pdata)
{
//	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	while(1)
	{ 
#if LWIP_DHCP									//当开启DHCP的时候
		if(lwipdev.dhcpstatus != 0) 			//开启DHCP
		{
			show_address(lwipdev.dhcpstatus );	//显示地址信息
			OSTaskSuspend(OS_PRIO_SELF); 		//显示完地址信息后挂起自身任务
		}
#else
		show_address(0); 						//显示静态地址
		
		OSTaskSuspend(OS_PRIO_SELF); 			//显示完地址信息后挂起自身任务
#endif //LWIP_DHCP
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//led任务
void led_task(void *pdata)
{
	pdata = pdata ;
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500);  //延时500ms
 	}
}

void mqtt_task(void *pdata)
{
	pdata = pdata ;
	printf("\r\ncJSON Version: %s\r\n", cJSON_Version());
	mqtt_thread();
//	while(1)
//	{
//		OSTimeDlyHMSM(0,0,0,500);  //
//	}
}

void dht11_task(void *pdata)
{
	pdata = pdata ;
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,500);  //
	}
}











