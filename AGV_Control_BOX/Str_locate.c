#include "Str_locate.h"

//************************************
// Method:    Find_Str
// FullName:  Find_Str
// Access:    public 
// Returns:   int	�����ַ���p��һ�γ��ֵ�λ�ã����������򷵻�-1
// Qualifier:
// Parameter: const char * source	�����ҵ��ַ�����
// Parameter: int source_length	Դ�ĳ���
// Parameter: const char * p	�����ҵ��ַ���,
// Description:	�ڳ���Ϊsource_length���ַ�����source�в����ַ���p(����'\0')��һ�γ��ֵ�λ��
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

//�ڳ���Ϊsource_length��source�ַ������в����ַ�p��λ��
//δ�ҵ��򷵻�-1
//source_length<=0Ҳ�᷵��-1
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