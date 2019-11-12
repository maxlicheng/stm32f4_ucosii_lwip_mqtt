# 说明

本项目基于正点原子STM32F4探索者开发板和阿里云平台的MQTT通信项目

## 使用
1.下载源码

2.修改mqtt_app.h文件中的阿里云设备三元数组

3.修改源码静态IP

4.编译后download到开发板

## 注意事项
1.务必自行在阿里云上创建物联网设备，并将相关三元参数填入mqtt_app.h文件中，否则功能无法正常演示

2.开发板网口需要用网线连入可上网的路由器，并修改源码中的静态IP参数，网段与路由器一致

3.开发板需要插入LCD显示屏，项目用的是4.3寸电阻屏

4.开发板需要接DS18B20温度传感器，用于上报环境温度

5.可用USB线接串口1，在串口终端上查看开发板运行状态

## 具体使用方式，可参考以下文章和视频
1.项目演示视频

https://www.bilibili.com/video/av52731912/

2.阿里云创建物联网设备教程

https://www.bilibili.com/video/av52731912/?p=2

3.MQTT协议简介

https://www.bilibili.com/video/av52731912/?p=3

4.基于github项目制作的MQTT点阵显示系统

https://www.bilibili.com/video/av52731912/?p=4

5.MQTT协议学习及调试利器——mqtt.fx

https://www.bilibili.com/video/av55897653/

6.个人博客

https://www.maxlicheng.com



