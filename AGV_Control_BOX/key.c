#include "key.h"

/*
* ʹ��״̬�����жϰ����Ƿ���
* ����״̬��һ���ֽ�����ʾ
* bit7:1��ʾ�������£�0��ʾ����δ����
* bit[1:0]	00 δ���£����ɿ���	01 ��⵽��������	10 ��������	11 ��⵽�����ɿ�
*/

//************************************
// Method:    Key_Scan
// FullName:  Key_Scan
// Access:    public
// Returns:   void
// Parameter: KEY * key	ָ�򰴼�״̬��ָ��
// Parameter: unsigned char value    ��⵽������״̬��1��ʾ���£�0��ʾδ����
// ˵��:	����״̬��ģ������ⰴ���Ƿ���
//************************************
void Key_Scan(KEY *key, unsigned char value)
{
	switch (key->key_state)
	{
	case 0: //����δ����
		if (value)
			key->key_state = 0x01; //��⵽��������
		break;
	case 1: //��⵽��������
		if (value)
			key->key_state = 0x02; //����ȷʵ����
		else
			key->key_state = 0x00; //����δ����
		break;
	case 2: //����ȷʵ����
		if (!value)
			key->key_state = 0x03; //��⵽�����ɿ�
		break;
	case 3: //��⵽�����ɿ�
		if (value)
			key->key_state = 0x02;
		else
		{
			key->pb = 1;
			key->key_state = 0x00; //����δ����
		}
		break;
	default:
		break;
	}
}
