#pragma once

typedef struct
{
	volatile unsigned pb : 1;	//pb=1��ʾ�������£�pb=0��ʾ����δ����
	volatile unsigned key_state : 2;	//��ʾ����״̬��00-δ��⵽�������£����������ɿ�����01-��⵽�������£�10-ȷ�ϰ������£�11-��⵽�����ɿ�
}KEY;

void Key_Scan(KEY *key,unsigned char value);	//����ɨ�裬�����ɿ�ʱ��PB��1