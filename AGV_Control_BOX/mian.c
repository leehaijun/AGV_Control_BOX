#include <stm32f0xx_gpio.h>
#include <stm32f0xx_rcc.h>
#include "delay.h"
#include "config.h"
#include "key.h"
#include "time.h"
#include "74HC165.h"
#include "74HC595.h"
#include "uart.h"
#include "C50XB.h"
#include "CRC16.h"

typedef struct
{
	KEY key;
	CMD_STATE cmd_state;		//�ð�ťָ���״̬
	char led_r_cnt;
	char led_b_cnt;	//����������ʱ��
	char beep_cnt;		//����������ʱ��
	unsigned char led_r;
	unsigned char led_b;		//ָʾ������״̬
	unsigned char beep;			//������״̬
	unsigned char rx_flag;		//����Ӧ���־��0-δ���յ�Ӧ��CMD_CALL-���պ���ָ���룬CMD_BACK-���շ���ָ���룬CMD_AGV_BUSY-AGV�豸æ
	unsigned char wf_cs;		//������־����ģ���Ƭѡ��1��ʾ�������ߵ͹���ģʽ
} BUTTON;

struct
{
	unsigned Power_LED : 1;
	unsigned Call_R : 1;
	unsigned Call_B : 1;
	unsigned Back_R : 1;
	unsigned Back_B : 1;
	unsigned C50XB_CS : 1;
	unsigned C50XB_Set : 1;
	unsigned Beep : 1;
}IO_HC595;	//�ȷ����λ
unsigned char const * const IO_Value = (unsigned char *)(&IO_HC595);

uint16_t Init_System(void);
void Work_Function(void);	//����״̬����
void Set_Function(void);	//����״̬����
void Init_Work(void);	//��ʼ������״̬ʱ�����ϵͳ����
void Get_ID(void);	//��ȡС����ַ��ͺ��к�վ���
void Init_WDT(void);	//��ʼ��ι�������IO(PA0)

void Check_Timeout(BUTTON *button);		//������ť���ơ���������ʱ
void Check_Station(BUTTON *station, unsigned char cmd);//������λ�İ����Ƿ���
void Send_CMD(unsigned char cmd); //����Ӧ��ָ��д�봮�ڻ�����
void Check_RX(void);

BUTTON Call = { .wf_cs = 0 }, Back = { .wf_cs = 0 };
C50XB_PROPERTY C50XB;

uint16_t DIP_switch;	//���ڱ��沦�뿪�ؼ�ֵ
uint32_t baudrate = 9600;	//���ڲ�����
unsigned char AGV_ADD = 0;	//AGVС�����е�ַ
unsigned char Subcode = 0;	//���к�վ��
uint16_t TIM14_Arr = 0;	//TIM14��װֵ�����ڼ��Modbus��
unsigned char CMD[5] = { 0xFF, 0, 0, 0, 0 };
unsigned char CRC16[3][2]; //���ڴ���CRCУ���� [0]-���гɹ���[1]-���з�æ��[2]-���سɹ�

char C50XB_flag = 0;	//���ڱ�־C50XB�Ƿ����
char _tim_flag = 0;	//���ڱ�ʾʱ���׼��50ms��ʱ���Ƿ��Ѿ��ﵽ
unsigned char frame_end = 0;        //1��ʾһ֡���ݽ�����ɣ�0��ʾδ���

int main(void)
{
	DIP_switch = Init_System();
#ifdef C50XB_DEBUG
	if (C50XB_flag == 0)	//C50XB������
	{
		IO_HC595.Beep = 1;
		HC595_Write_Byte(*IO_Value);
		HC595_Refresh();
		while (1)
		{
			//C50XB������������
		}
	}
#endif
	(DIP_switch == 0) ? Set_Function() : Work_Function();
}

//************************************
// Method:    Init_System
// FullName:  Init_System
// Access:    public 
// Returns:   uint16_t
// Parameter: void
// Description:	��ʼ��ϵͳ�����ز��뿪�ؼ�ֵ
//************************************
uint16_t Init_System(void)
{
	delay_init();
	Init_WDT();
	Init_TIM(8000, 10, TIM16, TIM16_IRQCHANNELPRIORITY);//��ʱʱ��10ms�����ڰ���ɨ�裬���Ź�ι��������ָʾ��
	TIM_Cmd(TIM16, ENABLE);	//��TIM16
	HC595_Init();
	HC165_Init();
	*(uint8_t*)(&IO_HC595) = 0;	//���˿�ֵȫ����0
	IO_HC595.Power_LED = 1;		//��Դָʾ����
	IO_HC595.C50XB_Set = 1;		//��������״̬
	HC595_Write_Byte(*IO_Value);
	HC595_Refresh();
	Uart_Init(9600);	//���ߴ�������״̬�Ĳ�����Ϊ9600
	delay_ms(1250);
	delay_ms(1250);	//��ʱ2.5sȷ��ʹC50XB��������״̬
	C50XB_flag = Get_C50XB_Property(&C50XB);	//��ȡC50XB����
	return HC165_Read(16) & 0x0FFF;	//���ز��뿪�ؼ�ֵ
}

void Work_Function(void)
{
	Init_Work();
	//frame_end = 0;
	while (1)
	{
		if (_tim_flag == 1)	//50msʱ����
		{
			_tim_flag = 0;
			Check_Timeout(&Call);
			Check_Timeout(&Back);
		}
		IO_HC595.Back_B = Back.led_b;
		IO_HC595.Back_R = Back.led_r;
		IO_HC595.Call_R = Call.led_r;
		IO_HC595.Call_B = Call.led_b;
		IO_HC595.Beep = Back.beep || Call.beep;
		IO_HC595.C50XB_CS = Back.wf_cs&&Call.wf_cs;
		HC595_Write_Byte(*IO_Value);
		HC595_Refresh();

		Check_Station(&Call, CMD_CALL);
		Check_Station(&Back, CMD_BACK);
		if (frame_end == 1) //���յ���һ֡����
		{
			Check_RX();
			rx_buf_cnt = 0; //������0
			frame_end = 0;
		}
	}
}

//��������ģʽ
void Set_Function(void)
{
	unsigned char Property;	//���õ�����
	unsigned char value;	//���õ�ֵ
	char set_c50xb_flag = 0;	//����C50XB�ı�־

	IO_HC595.Beep = 1;
	HC595_Write_Byte(*IO_Value);
	HC595_Refresh();
	delay_ms(1000);
	IO_HC595.Beep = 0;
	HC595_Write_Byte(*IO_Value);
	HC595_Refresh();

	while (1)
	{
		if (Call.key.pb == 1)	//��⵽����
		{
			DIP_switch = HC165_Read(16) & 0x0FFF;
			Property = (DIP_switch & 0x0F00) >> 8;
			value = DIP_switch & 0x00FF;
			switch (Property)
			{
			case SET_BAUDRATE:	//���ò�����
				switch (value)
				{
				case 	C50XB_BAUD_WF_RATE_1200:
				case	C50XB_BAUD_WF_RATE_2400:
				case	C50XB_BAUD_WF_RATE_4800:
				case	C50XB_BAUD_WF_RATE_9600:
				case	C50XB_BAUD_WF_RATE_14400:
				case	C50XB_BAUD_WF_RATE_19200:
				case	C50XB_BAUD_WF_RATE_38400:
				case	C50XB_BAUD_WF_RATE_57600:
				case	C50XB_BAUD_WF_RATE_76800:
				case	C50XB_BAUD_WF_RATE_115200:
					C50XB.baudrate = value;
					break;
				default:
					break;
				}
				break;
			case SET_WF_RATE:	//������������
				switch (value)
				{
				case 	C50XB_BAUD_WF_RATE_1200:
				case	C50XB_BAUD_WF_RATE_2400:
				case	C50XB_BAUD_WF_RATE_4800:
				case	C50XB_BAUD_WF_RATE_9600:
				case	C50XB_BAUD_WF_RATE_14400:
				case	C50XB_BAUD_WF_RATE_19200:
				case	C50XB_BAUD_WF_RATE_38400:
				case	C50XB_BAUD_WF_RATE_57600:
				case	C50XB_BAUD_WF_RATE_76800:
				case	C50XB_BAUD_WF_RATE_115200:
					C50XB.wf_rate = value;
					break;
				default:
					break;
				}
				break;
			case SET_CHANNEL:	//�����ŵ�
				if (value >= 1 && value <= 40)
				{
					C50XB.channel = value;
				}
				break;
			case SET_TRANS_POWER:	//���÷��书��
				if (value <= 7)
				{
					C50XB.trans_power = value;
				}
				break;
			case SET_OPERATE_FRE:	//���ù���Ƶ��
				if (value >= 1 && value <= 4)
				{
					C50XB.operate_frequency = value;
				}
				break;
			default:
				break;
			}	 //������Ӧ����
			set_c50xb_flag = Set_C50XB_Property(&C50XB);
			//����ʧ��
			if (set_c50xb_flag == 0)
			{
				IO_HC595.Beep = 1;
				HC595_Write_Byte(*IO_Value);
				HC595_Refresh();
				delay_ms(500);
				IO_HC595.Beep = 0;
				HC595_Write_Byte(*IO_Value);
				HC595_Refresh();
			}
			//���óɹ�
			else
			{
				IO_HC595.Beep = 1;
				HC595_Write_Byte(*IO_Value);
				HC595_Refresh();
				delay_ms(100);
				IO_HC595.Beep = 0;
				HC595_Write_Byte(*IO_Value);
				HC595_Refresh();
			}
			Call.key.pb = 0;
		}
	}
}

void Init_Work(void)
{
	unsigned short CRC16_temp;
	IO_HC595.C50XB_Set = 0;
	HC595_Write_Byte(*IO_Value);	//ʹ����ģ���������״̬
	HC595_Refresh();

	switch (C50XB.baudrate)
	{
	case C50XB_BAUD_WF_RATE_1200:
		baudrate = 1200;
		break;
	case C50XB_BAUD_WF_RATE_2400:
		baudrate = 2400;
		break;
	case C50XB_BAUD_WF_RATE_4800:
		baudrate = 4800;
		break;
	case C50XB_BAUD_WF_RATE_9600:
		baudrate = 9600;
		break;
	case C50XB_BAUD_WF_RATE_14400:
		baudrate = 14400;
		break;
	case C50XB_BAUD_WF_RATE_19200:
		baudrate = 19200;
		break;
	case C50XB_BAUD_WF_RATE_38400:
		baudrate = 38400;
		break;
	case C50XB_BAUD_WF_RATE_57600:
		baudrate = 57600;
		break;
	case C50XB_BAUD_WF_RATE_76800:
		baudrate = 76800;
		break;
	case C50XB_BAUD_WF_RATE_115200:
		baudrate = 115200;
		break;
	default:
		break;
	}   //���²�����
	Uart_Init(baudrate);

	Init_TIM(4000, 100, TIM3, TIM3_IRQCHANNELPRIORITY);//��ʱʱ��50ms�����ڵȴ�Ӧ��ʱ��⣬���ơ�������ʱ���жϣ�����ʵ��
	TIM_Cmd(TIM3, ENABLE);

	TIM14_Arr = (uint16_t)(SystemCoreClock / (baudrate / 22) / 10 + 1);	//���þ�Ĭʱ��Ϊ22��λ(��ʱ����Ƶϵ��10)
	Init_TIM(TIM14_Arr, 10, TIM14, TIM14_IRQCHANNELPRIORITY);//���ô���Modbus��ⳬʱ

	Get_ID();	//��ȡС����ַ���վ����

	CMD[3] = 0x00;
	CMD[4] = Subcode; //��ǰҪ��Ϊһ��һ��IDΪ0��subcodeΪ���뿪��
	CMD[2] = CMD_CALL;
	CMD[1] = AGV_ADD;
	CRC16_temp = CRC16_Cal(CMD, 5);
	CRC16[0][0] = CRC16_temp & 0x00FF;
	CRC16[0][1] = CRC16_temp >> 8;
	CMD[2] = CMD_AGV_BUSY;
	CRC16_temp = CRC16_Cal(CMD, 5);
	CRC16[1][0] = CRC16_temp & 0x00FF;
	CRC16[1][1] = CRC16_temp >> 8;
	CMD[2] = CMD_BACK;
	CRC16_temp = CRC16_Cal(CMD, 5);
	CRC16[2][0] = CRC16_temp & 0x00FF;
	CRC16[2][1] = CRC16_temp >> 8;

	for (int i = 0; i < 6; i++)
	{
		IO_HC595.Beep = ~IO_HC595.Beep;
		HC595_Write_Byte(*IO_Value);
		HC595_Refresh();
		delay_ms(200);
	}
}

void Get_ID(void)
{
	DIP_switch = HC165_Read(16) & 0x0FFF;
	AGV_ADD = (DIP_switch & 0x0F00) >> 8;
	Subcode = DIP_switch & 0x00FF;
}

void Init_WDT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Check_Timeout(BUTTON * button)
{
	if ((button->cmd_state.state == 1) && (button->cmd_state.timeout_num > 0))
	{
		button->cmd_state.timeout_num--;
	}

	if ((button->led_r_cnt) > 0)
	{
		button->led_r = 1;
		button->led_r_cnt--;
	}
	else
	{
		button->led_r = 0;
	}
	if ((button->led_b_cnt) > 0)
	{
		button->led_b = 1;
		button->led_b_cnt--;
	}
	else
	{
		button->led_b = 0;
	}
	if ((button->beep_cnt) > 0)
	{
		button->beep = 1;
		button->beep_cnt--;
	}
	else
	{
		button->beep = 0;
	}
}

void Check_Station(BUTTON * station, unsigned char cmd)
{
	CMD_STATE *cmd_state;

	cmd_state = &(station->cmd_state);
	if (station->key.pb == 1) //�а�������
	{
		switch (cmd_state->state)
		{
		case 0:                   //δ����ָ��
			Send_CMD(cmd);        //д����Ӧָ��
			cmd_state->state = 1; //�ѷ���ָ��ȵ�Ӧ��
			cmd_state->retry = RETRY - 1;
			cmd_state->timeout_num = RX_TIMEOUT;
			break;
		case 1: //�ѷ���ָ��ȴ�Ӧ��
			switch (station->rx_flag)
			{
			case 0:
				if (cmd_state->timeout_num == 0) //��ʱ
				{
					if (cmd_state->retry == 0) //���Դ����Ѵﵽ����
					{
						station->key.pb = 0; //���������־
						cmd_state->state = 0; //�л�״̬��δ����ָ��
						station->led_r_cnt = LED_TIME;
						//station->led_r = 1;
					}
					else //���Դ���δ�ﵽ����
					{
						Send_CMD(cmd); //д����Ӧָ��
						cmd_state->retry--;
						cmd_state->timeout_num = RX_TIMEOUT; //��ʱʱ�临λ
					}
				}
				break;
			case CMD_CALL:            //���յ�����ָ����
			case CMD_BACK:            //���յ�����ָ����
				cmd_state->state = 3; //ת������Ӧ��ɹ�
				station->led_b_cnt = LED_TIME;
				//station->led_b = 1;
				break;
			case CMD_AGV_BUSY:        //���յ�AGVæ
				cmd_state->state = 3; //ת������Ӧ��ɹ�
				station->led_b_cnt = LED_TIME;
				station->led_r_cnt = LED_TIME;
				//station->led_r = 1;
				//station->led_b = 1;
				break;
			default:
				break;
			}
			break;
		case 3: //����Ӧ��ɹ�
			station->rx_flag = 0;
			station->key.pb = 0;
			cmd_state->state = 0;
			break;
		default:
			break;
		}
	}
}

void Send_CMD(unsigned char cmd)
{
	switch (cmd) //����ָ��
	{
	case CMD_CALL:
		CMD[2] = CMD_CALL;
		uart1_send(CMD, 5);
		uart1_send(CRC16[0], 2);
		break;
	case CMD_BACK:
		CMD[2] = CMD_BACK;
		uart1_send(CMD, 5);
		uart1_send(CRC16[2], 2);
		break;
	default:
		break;
	}
}

void Check_RX(void)
{
	int i = 0, temp = 0;

	do
	{
		i += temp;
		if ((i != -1) && (Uart_RX_BUF[i + 1] == AGV_ADD) && (Uart_RX_BUF[i + 3] == 0) && (Uart_RX_BUF[i + 4] == Subcode) && ((char)(rx_buf_cnt - i) > 6))
		{
			switch (Uart_RX_BUF[i + 2])
			{
			case CMD_CALL:
				if ((Call.cmd_state.state == 1) && (Uart_RX_BUF[i + 5] == CRC16[0][0]) && (Uart_RX_BUF[i + 6] == CRC16[0][1]))
				{
					Call.rx_flag = CMD_CALL;
				}
				break;
			case CMD_BACK:
				if ((Back.cmd_state.state == 1) && (Uart_RX_BUF[i + 5] == CRC16[2][0]) && (Uart_RX_BUF[i + 6] == CRC16[2][1]))
				{
					Back.rx_flag = CMD_BACK;
				}
				break;
			case CMD_AGV_BUSY:
				if ((Call.cmd_state.state == 1) && (Uart_RX_BUF[i + 5] == CRC16[1][0]) && (Uart_RX_BUF[i + 6] == CRC16[1][1]))
				{
					Call.rx_flag = CMD_AGV_BUSY;
				}
				break;
			default:
				break;
			}
			break;
		}
		else
		{
			temp = i + 1;
			i = Find_Char(&Uart_RX_BUF[temp], rx_buf_cnt-temp, 0xFF);
		}
	} while (i != -1);
}


//����1		�����ڷ��ͽ���ģʽ���ж����ȼ�2
void USART1_IRQHandler(void)
{
	if (USART1->ISR&(1 << 5))	//�����ж�
	{
		TIM14->CNT = 0;	//��������0
		TIM14->CR1 |= TIM_CR1_CEN;	//ʹ�ܶ�ʱ��1
		Uart_RX_BUF[rx_buf_cnt] = USART1->RDR;
		rx_buf_cnt++;
	}
}

//��ʱ��3	���ڵȴ�Ӧ��ʱ��⣬���ơ�������ʱ���ж�, �ж����ȼ�4
void TIM3_IRQHandler(void)
{
	if (TIM3->SR&TIM_IT_Update)	//�����ж�
	{
		TIM3->SR = ~TIM_IT_Update;
		_tim_flag = 1;
	}
}

//��ʱ��14	���ڴ���Modbus��⣬�ж����ȼ�1
void TIM14_IRQHandler(void)
{
	if (TIM14->SR&TIM_IT_Update)	//�����ж�
	{
		TIM14->SR = ~TIM_IT_Update;
		TIM14->CR1 &= ~TIM_CR1_CEN;	//�رն�ʱ��14
		frame_end = 1;	//���ܵ���һ֡����
	}
}

//��ʱ��16	���ڰ���ɨ�裬���Ź�ι��, ����ָʾ�ƣ��ж����ȼ�3
void TIM16_IRQHandler(void)
{
	static uint16_t key_value = 0;
	static uint16_t _tim16_cnt = 0;
	if (TIM16->SR&TIM_IT_Update)	//�����ж�
	{
		key_value = HC165_Read(2);	//��ȡ��������״̬
		Key_Scan(&Call.key, key_value & 0x01);
		Key_Scan(&Back.key, key_value >> 1);
		if (Call.key.key_state==0x02)
		{
			Call.wf_cs = 0;
		}
		if (Back.key.key_state == 0x02)
		{
			Back.wf_cs = 0;
		}
		if ((_tim16_cnt % 5) == 0)
		{
			GPIOA->ODR ^= GPIO_Pin_0;
		}
		TIM16->SR = ~TIM_IT_Update;
	}
}