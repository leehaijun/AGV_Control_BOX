#include "C50XB.h"

#define READ_PROPERTY_CMD	"\xAA\xFA\x01"	//读参数指令
static unsigned char _property[17] = { 0xAA,0xFA,0x03,0x14,0x01,0x03,0x07,0x03,0x02,0x01,0x01 };

//************************************
// Method:    Get_C50XB_Property
// FullName:  Get_C50XB_Property
// Access:    public 
// Returns:   char	成功返回-1，失败返回0
// Qualifier:
// Parameter: C50XB_PROPERTY * c50xb
// Description:	获取C50XB无线模块的属性
//************************************
char Get_C50XB_Property(C50XB_PROPERTY * c50xb)
{
	int i = 0;
	uart1_send(READ_PROPERTY_CMD, 3);	//发送查询指令
	delay_ms(500);	//延迟500ms等待模块响应
 	if (rx_buf_cnt >= 16)
	{
		i = Find_Str(Uart_RX_BUF, rx_buf_cnt, "\r\n");
		if (i >= 14)
		{
			for (int j = 0; j < 8; j++)	//不设置NET ID和NODE ID
			{
				_property[j + 3] = Uart_RX_BUF[i - 14 + j];
			}
			c50xb->channel = _property[3];
			c50xb->operate_frequency = _property[4];
			c50xb->wf_rate = _property[5];
			c50xb->trans_power = _property[6];
			c50xb->baudrate = _property[7];
			rx_buf_cnt = 0;	//解析完数据将计数清0
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
// Returns:   char 成功返回-1，失败返回0
// Qualifier:
// Parameter: C50XB_PROPERTY * c50xb
// Description:	设置C50XB的属性
//************************************
char Set_C50XB_Property(C50XB_PROPERTY * c50xb)
{
	char i = 0;
	_property[3] = c50xb->channel;
	_property[4] = c50xb->operate_frequency;
	_property[5] = c50xb->wf_rate;
	_property[6] = c50xb->trans_power;
	_property[7] = c50xb->baudrate;
	uart1_send(_property, 17);	//发送设置
	delay_ms(500);
	i = Find_Str(Uart_RX_BUF, rx_buf_cnt, "ERROR\r\n");
	if ((char)(-1)==i)	//未找到ERROR
	{
		i = Find_Str(Uart_RX_BUF, rx_buf_cnt, "OK\r\n");
		if (i!=-1)
		{
			rx_buf_cnt = 0;
			return -1;	//成功找到
		}
	}
	rx_buf_cnt = 0;
	return 0;
}
