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
* ���ߴ���ģ��C50XB��������
*/
typedef struct
{
	C50XB_BAUD_WF_RATE baudrate;	//������
	C50XB_BAUD_WF_RATE wf_rate;	//��������
	uint8_t channel;			//�ŵ�
	uint8_t trans_power;	//���书��
	uint8_t operate_frequency;	//����Ƶ��
}C50XB_PROPERTY;

typedef enum
{
	SET_BAUDRATE,		//���ò�����
	SET_WF_RATE,		//������������
	SET_CHANNEL,		//�����ŵ�
	SET_TRANS_POWER,	//���÷��书��
	SET_OPERATE_FRE		//���ù���Ƶ��
}SET_C50XB_STATE;

char Get_C50XB_Property(C50XB_PROPERTY *c50xb);	//��ȡC50XB����
char Set_C50XB_Property(C50XB_PROPERTY *c50xb);	//����C50XB����