#include "netif/ethernetif.h" 
#include "lan8720.h"  
#include "lwip_comm.h" 
#include "netif/etharp.h"  
#include "string.h"  

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//ethernetif.c 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/8/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
 
//由ethernetif_init()调用用于初始化硬件
//netif:网卡结构体指针 
//返回值:ERR_OK,正常
//       其他,失败
static err_t low_level_init(struct netif *netif)
{
#ifdef CHECKSUM_BY_HARDWARE
	int i; 
#endif 
	netif->hwaddr_len = ETHARP_HWADDR_LEN; //设置MAC地址长度,为6个字节
	//初始化MAC地址,设置什么地址由用户自己设置,但是不能与网络中其他设备MAC地址重复
	netif->hwaddr[0]=lwipdev.mac[0]; 
	netif->hwaddr[1]=lwipdev.mac[1]; 
	netif->hwaddr[2]=lwipdev.mac[2];
	netif->hwaddr[3]=lwipdev.mac[3];
	netif->hwaddr[4]=lwipdev.mac[4];
	netif->hwaddr[5]=lwipdev.mac[5];
	netif->mtu=1500; //最大允许传输单元,允许该网卡广播和ARP功能
	//并且该网卡允许有硬件链路连接
	netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP;
	
	ETH_MACAddressConfig(ETH_MAC_Address0, netif->hwaddr); //向STM32F4的MAC地址寄存器中写入MAC地址
	ETH_DMATxDescChainInit(DMATxDscrTab, Tx_Buff, ETH_TXBUFNB);
	ETH_DMARxDescChainInit(DMARxDscrTab, Rx_Buff, ETH_RXBUFNB);
#ifdef CHECKSUM_BY_HARDWARE 	//使用硬件帧校验
	for(i=0;i<ETH_TXBUFNB;i++)	//使能TCP,UDP和ICMP的发送帧校验,TCP,UDP和ICMP的接收帧校验在DMA中配置了
	{
		ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
	}
#endif
	ETH_Start(); //开启MAC和DMA				
	return ERR_OK;
} 
//用于发送数据包的最底层函数(lwip通过netif->linkoutput指向该函数)
//netif:网卡结构体指针
//p:pbuf数据结构体指针
//返回值:ERR_OK,发送正常
//       ERR_MEM,发送失败
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	u8 res;
	struct pbuf *q;
	int l = 0;
	u8 *buffer=(u8 *)ETH_GetCurrentTxBuffer(); 
	for(q=p;q!=NULL;q=q->next) 
	{
		memcpy((u8_t*)&buffer[l], q->payload, q->len);
		l=l+q->len;
	} 
	res=ETH_Tx_Packet(l); 
	if(res==ETH_ERROR)return ERR_MEM;//返回错误状态
	return ERR_OK;
} 
//用于接收数据包的最底层函数
//neitif:网卡结构体指针
//返回值:pbuf数据结构体指针
static struct pbuf * low_level_input(struct netif *netif)
{  
	struct pbuf *p, *q;
	u16_t len;
	int l =0;
	FrameTypeDef frame;
	u8 *buffer;
	p = NULL;
	frame=ETH_Rx_Packet();
	len=frame.length;//得到包大小
	buffer=(u8 *)frame.buffer;//得到包数据地址 
	p=pbuf_alloc(PBUF_RAW,len,PBUF_POOL);//pbufs内存池分配pbuf
	if(p!=NULL)
	{
		for(q=p;q!=NULL;q=q->next)
		{
			memcpy((u8_t*)q->payload,(u8_t*)&buffer[l], q->len);
			l=l+q->len;
		}    
	}
	frame.descriptor->Status=ETH_DMARxDesc_OWN;//设置Rx描述符OWN位,buffer重归ETH DMA 
	if((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)//当Rx Buffer不可用位(RBUS)被设置的时候,重置它.恢复传输
	{ 
		ETH->DMASR=ETH_DMASR_RBUS;//重置ETH DMA RBUS位 
		ETH->DMARPDR=0;//恢复DMA接收
	}
	return p;
} 
//网卡接收数据(lwip直接调用)
//netif:网卡结构体指针
//返回值:ERR_OK,发送正常
//       ERR_MEM,发送失败
err_t ethernetif_input(struct netif *netif)
{
	err_t err;
	struct pbuf *p;
	p=low_level_input(netif);
	if(p==NULL) return ERR_MEM;
	err=netif->input(p, netif);
	if(err!=ERR_OK)
	{
		LWIP_DEBUGF(NETIF_DEBUG,("ethernetif_input: IP input error\n"));
		pbuf_free(p);
		p = NULL;
	} 
	return err;
} 
//使用low_level_init()函数来初始化网络
//netif:网卡结构体指针
//返回值:ERR_OK,正常
//       其他,失败
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif!=NULL",(netif!=NULL));
#if LWIP_NETIF_HOSTNAME			//LWIP_NETIF_HOSTNAME 
	netif->hostname="lwip";  	//初始化名称
#endif 
	netif->name[0]=IFNAME0; 	//初始化变量netif的name字段
	netif->name[1]=IFNAME1; 	//在文件外定义这里不用关心具体值
	netif->output=etharp_output;//IP层发送数据包函数
	netif->linkoutput=low_level_output;//ARP模块发送数据包函数
	low_level_init(netif); 		//底层硬件初始化函数
	return ERR_OK;
}














