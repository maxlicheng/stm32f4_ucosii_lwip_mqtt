/**********************************************************************************************************
** 文件名		:mqtt_app.h
** 作者			:maxlicheng<licheng.chn@outlook.com>
** 作者github	:https://github.com/maxlicheng
** 作者博客		:https://www.maxlicheng.com/	
** 生成日期		:2018-08-08
** 描述			:mqtt服务程序头文件
************************************************************************************************************/
#ifndef _MQTT_APP_H_
#define _MQTT_APP_H_
#include "sys.h"

//用户需要根据设备信息完善以下宏定义中的四元组内容
#define PRODUCT_KEY    	"a1Yjxb6GjGk"															//阿里云颁发的产品唯一标识，11位长度的英文数字随机组合
#define DEVICE_NAME    	"mqtt_test"																//用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一
#define DEVICE_SECRET  	"vfW2KtmvfGy9AcBwNY9h4wksJifwt2Lf"				//设备密钥，与DeviceName成对出现，可用于一机一密的认证方案

//#define PRODUCT_SECRET 	"a1L5lKy2Cpn"														//阿里云颁发的产品加密密钥，通常与ProductKey成对出现，可用于一型一密的认证方案


//以下宏定义固定，不需要修改
#define HOST_NAME  			PRODUCT_KEY".iot-as-mqtt.cn-shanghai.aliyuncs.com"															//阿里云域名
#define HOST_PORT 			1883																																						//阿里云域名端口，固定1883
#define CONTENT				"clientId"DEVICE_NAME"deviceName"DEVICE_NAME"productKey"PRODUCT_KEY"timestamp789"	//计算登录密码用
#define CLIENT_ID			DEVICE_NAME"|securemode=3,signmethod=hmacsha1,timestamp=789|"											//客户端ID
#define USER_NAME			DEVICE_NAME"&"PRODUCT_KEY																													//客户端用户名
//#define PASSWORD			"AA6A749E740A3019D58090FF3ADC57B9DB4B380E"																			//客户端登录password通过hmac_sha1算法得到，大小写不敏感
#define DEVICE_PUBLISH		"/sys/"PRODUCT_KEY"/"DEVICE_NAME"/thing/event/property/post"									//
#define DEVICE_SUBSCRIBE	"/sys/"PRODUCT_KEY"/"DEVICE_NAME"/thing/service/property/set"									//设置设备属性

//以下三个TOPIC的宏定义不需要用户修改，可以直接使用
//IOT HUB为设备建立三个TOPIC：update用于设备发布消息，error用于设备发布错误，get用于订阅消息
//#define TOPIC_UPDATE         "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
//#define TOPIC_ERROR          "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
//#define TOPIC_GET            "/"PRODUCT_KEY"/"DEVICE_NAME"/get"

void mqtt_thread(void);
void HexToStr(uint8_t *pbDest, uint8_t *pbSrc, int nLen);																		//数值转16进制字符串
void getPassword(const char *device_secret, const char *content, char *password);						//用户密码获取
//u32 PublishData(float temp, float humid, unsigned char *payloadbuf);

#endif



