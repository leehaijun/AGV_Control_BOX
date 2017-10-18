#pragma once
#include <string.h>

int Find_Str(const char *source,int source_length, const char *p);	

//在长度为source_length的source字符数组中查找字符p的位置
//未找到则返回-1
//source_length<=0也会返回-1
int Find_Char(const unsigned char *source, int source_length, const unsigned char p);