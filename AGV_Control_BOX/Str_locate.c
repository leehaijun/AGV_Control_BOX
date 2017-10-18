#include "Str_locate.h"

//************************************
// Method:    Find_Str
// FullName:  Find_Str
// Access:    public 
// Returns:   int	返回字符串p第一次出现的位置，若不存在则返回-1
// Qualifier:
// Parameter: const char * source	待查找的字符数组
// Parameter: int source_length	源的长度
// Parameter: const char * p	待查找的字符串,
// Description:	在长度为source_length的字符数组source中查找字符串p(忽略'\0')第一次出现的位置
//************************************
int Find_Str(const char * source, int source_length, const char * p)
{
	const char *char1 = source;
	const char *char2 = p;
	int i = 0;
	const int p_length = strlen(p);
	if (source_length <= 0 || (p == 0))
	{
		return -1;
	}

	for (i = 0; i <= (source_length - p_length); i++, source++)
	{
		if (*source == *p)
		{
			char1 = source;
			char2 = p;
			while ((*char1 == *char2) && (*char2))
			{
				char1++;
				char2++;
			}
			if (*char2 == 0)
			{
				return i;
			}
		}
	}
	return -1;
}

//在长度为source_length的source字符数组中查找字符p的位置
//未找到则返回-1
//source_length<=0也会返回-1
int Find_Char(const unsigned char *source, int source_length, const unsigned char p)
{
	char i = 0;
	while (source_length > 0)
	{
		if (*source == p)
		{
			return i;
		}
		else
		{
			source++;
			source_length--;
			i++;
		}
	}
	return -1;
}