#include "C50XB.h"

#define READ_PROPERTY_CMD	"\xAA\xFA\x01"	//������ָ��
static unsigned char _property[17] = { 0xAA,0xFA,0x03,0x14,0x01,0x03,0x07,0x03,0x02,0x01,0x01 };

//************************************
// Method:    Get_C50XB_Property
// FullName:  Get_C50XB_Property
// Access:    public 
// Returns:   char	�ɹ�����-1��ʧ�ܷ���0
// Qualifier:
// Parameter: C50XB_PROPERTY * c50xb
// Description:	��ȡC50XB����ģ�������
//************************************
char Get_C50XB_Property(C50XB_PROPERTY * c50xb)
{
	int i = 0;
	uart1_send(READ_PROPERTY_CMD, 3);	//���Ͳ�ѯָ��
	delay_ms(500);	//�ӳ�500ms�ȴ�ģ����Ӧ
 	if (rx_buf_cnt >= 16)
	{
		i = Find_Str(Uart_RX_BUF, rx_buf_cnt, "\r\n");
		if (i >= 14)
		{
			for (int j = 0; j < 8; j++)	//������NET ID��NODE ID
			{
				_property[j + 3] = Uart_RX_BUF[i - 14 + j];
			}
			c50xb->channel = _property[3];
			c50xb->operate_frequency = _property[4];
			c50xb->wf_rate = _property[5];
			c50xb->trans_power = _property[6];
			c50xb->baudrate = _property[7];
			rx_buf_cnt = 0;	//���������ݽ�������0
			return -1;
		}
		else
		{
			rx_buf_cnt = 0;
			return 0;
		}
	}
	return 0;
}

//************************************
// Method:    Set_C50XB_Property
// FullName:  Set_C50XB_Property
// Access:    public 
// Returns:   char �ɹ�����-1��ʧ�ܷ���0
// Qualifier:
// Parameter: C50XB_PROPERTY * c50xb
// Description:	����C50XB������
//************************************
char Set_C50XB_Property(C50XB_PROPERTY * c50xb)
{
	char i = 0;
	_property[3] = c50xb->channel;
	_property[4] = c50xb->operate_frequency;
	_property[5] = c50xb->wf_rate;
	_property[6] = c50xb->trans_power;
	_property[7] = c50xb->baudrate;
	uart1_send(_property, 17);	//��������
	delay_ms(500);
	i = Find_Str(Uart_RX_BUF, rx_buf_cnt, "ERROR\r\n");
	if ((char)(-1)==i)	//δ�ҵ�ERROR
	{
		i = Find_Str(Uart_RX_BUF, rx_buf_cnt, "OK\r\n");
		if (i!=-1)
		{
			rx_buf_cnt = 0;
			return -1;	//�ɹ��ҵ�
		}
	}
	rx_buf_cnt = 0;
	return 0;
}
