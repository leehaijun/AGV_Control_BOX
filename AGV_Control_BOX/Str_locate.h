#pragma once
#include <string.h>

int Find_Str(const char *source,int source_length, const char *p);	

//�ڳ���Ϊsource_length��source�ַ������в����ַ�p��λ��
//δ�ҵ��򷵻�-1
//source_length<=0Ҳ�᷵��-1
int Find_Char(const unsigned char *source, int source_length, const unsigned char p);