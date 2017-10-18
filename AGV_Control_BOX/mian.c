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
	CMD_STATE cmd_state;		//该按钮指令发送状态
	char led_r_cnt;
	char led_b_cnt;	//红蓝灯亮灯时间
	char beep_cnt;		//蜂鸣器鸣响时间
	unsigned char led_r;
	unsigned char led_b;		//指示红蓝灯状态
	unsigned char beep;			//蜂鸣器状态
	unsigned char rx_flag;		//接收应答标志，0-未接收到应答，CMD_CALL-接收呼叫指令码，CMD_BACK-接收返回指令码，CMD_AGV_BUSY-AGV设备忙
	unsigned char wf_cs;		//用来标志无线模块的片选，1表示进入休眠低功耗模式
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
}IO_HC595;	//先分配低位
unsigned char const * const IO_Value = (unsigned char *)(&IO_HC595);

uint16_t Init_System(void);
void Work_Function(void);	//工作状态函数
void Set_Function(void);	//设置状态函数
void Init_Work(void);	//初始化工作状态时所需的系统参数
void Get_ID(void);	//获取小车地址码和呼叫盒站点号
void Init_WDT(void);	//初始化喂狗所需的IO(PA0)

void Check_Timeout(BUTTON *button);		//检查各按钮亮灯、蜂鸣器超时
void Check_Station(BUTTON *station, unsigned char cmd);//检查各工位的按键是否按下
void Send_CMD(unsigned char cmd); //将相应的指令写入串口缓存区
void Check_RX(void);

BUTTON Call = { .wf_cs = 0 }, Back = { .wf_cs = 0 };
C50XB_PROPERTY C50XB;

uint16_t DIP_switch;	//用于保存拨码开关键值
uint32_t baudrate = 9600;	//串口波特率
unsigned char AGV_ADD = 0;	//AGV小车呼叫地址
unsigned char Subcode = 0;	//呼叫盒站点
uint16_t TIM14_Arr = 0;	//TIM14重装值（用于检测Modbus）
unsigned char CMD[5] = { 0xFF, 0, 0, 0, 0 };
unsigned char CRC16[3][2]; //用于储存CRC校验码 [0]-呼叫成功、[1]-呼叫繁忙、[2]-返回成功

char C50XB_flag = 0;	//用于标志C50XB是否出错
char _tim_flag = 0;	//用于表示时间基准（50ms）时间是否已经达到
unsigned char frame_end = 0;        //1表示一帧数据接收完成，0表示未完成

int main(void)
{
	DIP_switch = Init_System();
#ifdef C50XB_DEBUG
	if (C50XB_flag == 0)	//C50XB检测出错
	{
		IO_HC595.Beep = 1;
		HC595_Write_Byte(*IO_Value);
		HC595_Refresh();
		while (1)
		{
			//C50XB报错，蜂鸣器响
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
// Description:	初始化系统，返回拨码开关键值
//************************************
uint16_t Init_System(void)
{
	delay_init();
	Init_WDT();
	Init_TIM(8000, 10, TIM16, TIM16_IRQCHANNELPRIORITY);//定时时间10ms，用于按键扫描，看门狗喂狗，电量指示灯
	TIM_Cmd(TIM16, ENABLE);	//打开TIM16
	HC595_Init();
	HC165_Init();
	*(uint8_t*)(&IO_HC595) = 0;	//将端口值全部清0
	IO_HC595.Power_LED = 1;		//电源指示灯亮
	IO_HC595.C50XB_Set = 1;		//进入设置状态
	HC595_Write_Byte(*IO_Value);
	HC595_Refresh();
	Uart_Init(9600);	//无线串口设置状态的波特率为9600
	delay_ms(1250);
	delay_ms(1250);	//延时2.5s确保使C50XB进入设置状态
	C50XB_flag = Get_C50XB_Property(&C50XB);	//读取C50XB参数
	return HC165_Read(16) & 0x0FFF;	//返回拨码开关键值
}

void Work_Function(void)
{
	Init_Work();
	//frame_end = 0;
	while (1)
	{
		if (_tim_flag == 1)	//50ms时基到
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
		if (frame_end == 1) //接收到了一帧数据
		{
			Check_RX();
			rx_buf_cnt = 0; //计数清0
			frame_end = 0;
		}
	}
}

//进入设置模式
void Set_Function(void)
{
	unsigned char Property;	//设置的属性
	unsigned char value;	//设置的值
	char set_c50xb_flag = 0;	//设置C50XB的标志

	IO_HC595.Beep = 1;
	HC595_Write_Byte(*IO_Value);
	HC595_Refresh();
	delay_ms(1000);
	IO_HC595.Beep = 0;
	HC595_Write_Byte(*IO_Value);
	HC595_Refresh();

	while (1)
	{
		if (Call.key.pb == 1)	//检测到按键
		{
			DIP_switch = HC165_Read(16) & 0x0FFF;
			Property = (DIP_switch & 0x0F00) >> 8;
			value = DIP_switch & 0x00FF;
			switch (Property)
			{
			case SET_BAUDRATE:	//设置波特率
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
			case SET_WF_RATE:	//设置无线速率
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
			case SET_CHANNEL:	//设置信道
				if (value >= 1 && value <= 40)
				{
					C50XB.channel = value;
				}
				break;
			case SET_TRANS_POWER:	//设置发射功率
				if (value <= 7)
				{
					C50XB.trans_power = value;
				}
				break;
			case SET_OPERATE_FRE:	//设置工作频段
				if (value >= 1 && value <= 4)
				{
					C50XB.operate_frequency = value;
				}
				break;
			default:
				break;
			}	 //设置相应属性
			set_c50xb_flag = Set_C50XB_Property(&C50XB);
			//设置失败
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
			//设置成功
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
	HC595_Write_Byte(*IO_Value);	//使无线模块进入设置状态
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
	}   //更新波特率
	Uart_Init(baudrate);

	Init_TIM(4000, 100, TIM3, TIM3_IRQCHANNELPRIORITY);//定时时间50ms，用于等待应答超时检测，亮灯、蜂鸣器时间判断，用作实基
	TIM_Cmd(TIM3, ENABLE);

	TIM14_Arr = (uint16_t)(SystemCoreClock / (baudrate / 22) / 10 + 1);	//设置静默时间为22个位(定时器分频系数10)
	Init_TIM(TIM14_Arr, 10, TIM14, TIM14_IRQCHANNELPRIORITY);//设置串口Modbus检测超时

	Get_ID();	//获取小车地址码和站点码

	CMD[3] = 0x00;
	CMD[4] = Subcode; //当前要求为一拖一，ID为0，subcode为拨码开关
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
	if (station->key.pb == 1) //有按键按下
	{
		switch (cmd_state->state)
		{
		case 0:                   //未发送指令
			Send_CMD(cmd);        //写入相应指令
			cmd_state->state = 1; //已发送指令，等到应答
			cmd_state->retry = RETRY - 1;
			cmd_state->timeout_num = RX_TIMEOUT;
			break;
		case 1: //已发送指令，等待应答
			switch (station->rx_flag)
			{
			case 0:
				if (cmd_state->timeout_num == 0) //超时
				{
					if (cmd_state->retry == 0) //重试次数已达到上限
					{
						station->key.pb = 0; //清除按键标志
						cmd_state->state = 0; //切换状态到未发送指令
						station->led_r_cnt = LED_TIME;
						//station->led_r = 1;
					}
					else //重试次数未达到上限
					{
						Send_CMD(cmd); //写入相应指令
						cmd_state->retry--;
						cmd_state->timeout_num = RX_TIMEOUT; //超时时间复位
					}
				}
				break;
			case CMD_CALL:            //接收到呼叫指令码
			case CMD_BACK:            //接收到返回指令码
				cmd_state->state = 3; //转到接收应答成功
				station->led_b_cnt = LED_TIME;
				//station->led_b = 1;
				break;
			case CMD_AGV_BUSY:        //接收到AGV忙
				cmd_state->state = 3; //转到接收应答成功
				station->led_b_cnt = LED_TIME;
				station->led_r_cnt = LED_TIME;
				//station->led_r = 1;
				//station->led_b = 1;
				break;
			default:
				break;
			}
			break;
		case 3: //接收应答成功
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
	switch (cmd) //发送指令
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


//串口1		工作在发送接收模式，中断优先级2
void USART1_IRQHandler(void)
{
	if (USART1->ISR&(1 << 5))	//接受中断
	{
		TIM14->CNT = 0;	//计数器清0
		TIM14->CR1 |= TIM_CR1_CEN;	//使能定时器1
		Uart_RX_BUF[rx_buf_cnt] = USART1->RDR;
		rx_buf_cnt++;
	}
}

//定时器3	用于等待应答超时检测，亮灯、蜂鸣器时间判断, 中断优先级4
void TIM3_IRQHandler(void)
{
	if (TIM3->SR&TIM_IT_Update)	//更新中断
	{
		TIM3->SR = ~TIM_IT_Update;
		_tim_flag = 1;
	}
}

//定时器14	用于串口Modbus检测，中断优先级1
void TIM14_IRQHandler(void)
{
	if (TIM14->SR&TIM_IT_Update)	//更新中断
	{
		TIM14->SR = ~TIM_IT_Update;
		TIM14->CR1 &= ~TIM_CR1_CEN;	//关闭定时器14
		frame_end = 1;	//接受到了一帧数据
	}
}

//定时器16	用于按键扫描，看门狗喂狗, 电量指示灯，中断优先级3
void TIM16_IRQHandler(void)
{
	static uint16_t key_value = 0;
	static uint16_t _tim16_cnt = 0;
	if (TIM16->SR&TIM_IT_Update)	//更新中断
	{
		key_value = HC165_Read(2);	//读取两个按键状态
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