#include "key.h"

/*
* 使用状态机来判断按键是否按下
* 按键状态用一个字节来表示
* bit7:1表示按键按下，0表示按键未按下
* bit[1:0]	00 未按下（已松开）	01 检测到按键按下	10 按键按下	11 检测到按键松开
*/

//************************************
// Method:    Key_Scan
// FullName:  Key_Scan
// Access:    public
// Returns:   void
// Parameter: KEY * key	指向按键状态的指针
// Parameter: unsigned char value    检测到按键的状态，1表示按下，0表示未按下
// 说明:	利用状态机模型来检测按键是否按下
//************************************
void Key_Scan(KEY *key, unsigned char value)
{
	switch (key->key_state)
	{
	case 0: //按键未按下
		if (value)
			key->key_state = 0x01; //检测到按键按下
		break;
	case 1: //检测到按键按下
		if (value)
			key->key_state = 0x02; //按键确实按下
		else
			key->key_state = 0x00; //按键未按下
		break;
	case 2: //按键确实按下
		if (!value)
			key->key_state = 0x03; //检测到按键松开
		break;
	case 3: //检测到按键松开
		if (value)
			key->key_state = 0x02;
		else
		{
			key->pb = 1;
			key->key_state = 0x00; //按键未按下
		}
		break;
	default:
		break;
	}
}
