#pragma once

typedef struct
{
	volatile unsigned pb : 1;	//pb=1表示按键按下，pb=0表示按键未按下
	volatile unsigned key_state : 2;	//表示按键状态，00-未检测到按键按下（或者是已松开），01-检测到按键按下，10-确认按键按下，11-检测到按键松开
}KEY;

void Key_Scan(KEY *key,unsigned char value);	//按键扫描，按键松开时，PB置1