#pragma once
/*
* STM32F030F4��5����ʱ����Tim1��3��14��16��17
* ��ʱ������
* ��ʱ��1	����
* ��ʱ��3	���ڵȴ�Ӧ��ʱ��⣬���ơ�������ʱ���ж�,�ж����ȼ�4
* ��ʱ��14	���ڴ���Modbus��⣬�ж����ȼ�1
* ��ʱ��16	���ڰ���ɨ�裬���Ź�ι��,����ָʾ�ƣ�State_LED�����ж����ȼ�3
* ��ʱ��17	����

* ����1		�����ڷ��ͽ���ģʽ���ж����ȼ�2
*/

#define TIM3_IRQCHANNELPRIORITY		4
#define TIM14_IRQCHANNELPRIORITY	1
#define TIM16_IRQCHANNELPRIORITY	3
#define UART1_IRQCHANNELPRIORITY	2


//һ������������50ms
#define RX_TIMEOUT	20 //����Ӧ��ʱ�����
#define LED_TIME	80   //LED����ʱ�����
#define BEEP_SHORT_TIME	2	//��������ʱ�����
#define BEEP_LONG_TIME	10	//��������ʱ�����
#define RETRY 5       //������Դ���

#define CMD_CALL 0x09     //����ָ����
#define CMD_BACK 0x08     //����ָ����
#define CMD_AGV_BUSY 0x89 //AGV�豸æ

#define C50XB_DEBUG

typedef struct
{
	//00 δ����ָ��	01 �ѷ���ָ��ȴ�Ӧ��	10 ����	11 ����Ӧ��ɹ�
	unsigned state : 2;
	//��ʾ���Դ�������
	unsigned retry : 5;
	//��ʾ��ʱʱ������
	unsigned timeout_num : 5;
} CMD_STATE; //���ڱ�ʾָ���״̬