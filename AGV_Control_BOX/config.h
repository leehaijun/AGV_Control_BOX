#pragma once
/*
* STM32F030F4有5个定时器，Tim1、3、14、16、17
* 定时器分配
* 定时器1	空闲
* 定时器3	用于等待应答超时检测，亮灯、蜂鸣器时间判断,中断优先级4
* 定时器14	用于串口Modbus检测，中断优先级1
* 定时器16	用于按键扫描，看门狗喂狗,电量指示灯（State_LED），中断优先级3
* 定时器17	空闲

* 串口1		工作在发送接收模式，中断优先级2
*/

#define TIM3_IRQCHANNELPRIORITY		4
#define TIM14_IRQCHANNELPRIORITY	1
#define TIM16_IRQCHANNELPRIORITY	3
#define UART1_IRQCHANNELPRIORITY	2


//一个计数周期是50ms
#define RX_TIMEOUT	20 //接收应答时间计数
#define LED_TIME	80   //LED亮灯时间计数
#define BEEP_SHORT_TIME	2	//蜂鸣器响时间计数
#define BEEP_LONG_TIME	10	//蜂鸣器响时间计数
#define RETRY 5       //最大重试次数

#define CMD_CALL 0x09     //呼叫指令码
#define CMD_BACK 0x08     //返回指令码
#define CMD_AGV_BUSY 0x89 //AGV设备忙

#define C50XB_DEBUG

typedef struct
{
	//00 未发送指令	01 已发送指令，等待应答	10 保留	11 接收应答成功
	unsigned state : 2;
	//表示重试次数限制
	unsigned retry : 5;
	//表示超时时间限制
	unsigned timeout_num : 5;
} CMD_STATE; //用于表示指令发送状态