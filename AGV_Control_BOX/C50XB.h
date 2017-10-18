#pragma once
#include "uart.h"
#include "delay.h"
#include "Str_locate.h"

typedef enum
{
	C50XB_BAUD_WF_RATE_1200 = 0,
	C50XB_BAUD_WF_RATE_2400,
	C50XB_BAUD_WF_RATE_4800,
	C50XB_BAUD_WF_RATE_9600,
	C50XB_BAUD_WF_RATE_14400,
	C50XB_BAUD_WF_RATE_19200,
	C50XB_BAUD_WF_RATE_38400,
	C50XB_BAUD_WF_RATE_57600,
	C50XB_BAUD_WF_RATE_76800,
	C50XB_BAUD_WF_RATE_115200
}C50XB_BAUD_WF_RATE;

/*
* 无线串口模块C50XB驱动程序
*/
typedef struct
{
	C50XB_BAUD_WF_RATE baudrate;	//波特率
	C50XB_BAUD_WF_RATE wf_rate;	//无线速率
	uint8_t channel;			//信道
	uint8_t trans_power;	//发射功率
	uint8_t operate_frequency;	//工作频段
}C50XB_PROPERTY;

typedef enum
{
	SET_BAUDRATE,		//设置波特率
	SET_WF_RATE,		//设置无线速率
	SET_CHANNEL,		//设置信道
	SET_TRANS_POWER,	//设置发射功率
	SET_OPERATE_FRE		//设置工作频段
}SET_C50XB_STATE;

char Get_C50XB_Property(C50XB_PROPERTY *c50xb);	//获取C50XB属性
char Set_C50XB_Property(C50XB_PROPERTY *c50xb);	//设置C50XB属性