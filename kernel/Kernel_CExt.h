#pragma once
// ---------------------------------------------------------------			
// Kernel_CExt.h
// ---------------------------------------------------------------	
// Operating System Kernel Extension Header
// OS内核 - 函数扩展 (汇编好难TAT)
// By Aeroraven, 2021-4-3
// --------------------------------------------------------------

#include "Kernel_CDef.h"
#include "Kernel_IncFunc.h"

//----------------------------
//    输出设置
//----------------------------

CONST DWORD CGA_COLMAX = 80; //CGA最大行
CONST UBYTE CGA_DEFAULT_COLOR = 0x3f;


//【KCEX_PrintChar】
// 打印字符
// 参数：UBYTE character 要打印的字符
VOID KCEX_PrintChar(UBYTE character) {
	DWORD CGA_CURRENTCOL = 0; //CGA当前列
	if (character != '\n') {
		AF_DispChar((UDWORD)character, CGA_DEFAULT_COLOR);
		AF_GetDispPos(&CGA_CURRENTCOL);
		if (CGA_CURRENTCOL >= CGA_COLMAX) {
			CGA_CURRENTCOL = 0;
		}
		return;
	}
	AF_GetDispPos(&CGA_CURRENTCOL);
	DWORD CGA_Remain = CGA_CURRENTCOL % (CGA_COLMAX * 2);
	for (DWORD i = 0; i < (CGA_COLMAX * 2 - CGA_Remain) / 2; i++) {
		AF_DispChar(' ', CGA_DEFAULT_COLOR);
	}
}

//【KCEX_PrintString】
// 打印字符
// 参数：UBYTE* chString 要打印的字符串
VOID KCEX_PrintString(UBYTE* chString) {
	UBYTE* i = chString;
	while (*i != '\0') {
		KCEX_PrintChar(*i);
		i++;
	}
}

//【KCEX_PrintByteHex】
// 打印一个字节(十六进制)
// 参数：UBYTE arg 要打印的字节
VOID KCEX_PrintByteHex(UBYTE arg) {
	UBYTE convStr[4];
	UBYTE temp;
	temp = arg & 0x0f;
	if (temp < 0x0a) {
		convStr[1] = '0' + temp;
	}
	else {
		convStr[1] = 'A' - 10 + temp;
	}
	temp = (arg & 0xf0) >> 4;
	if (temp < 0x0a) {
		convStr[0] = '0' + temp;
	}
	else {
		convStr[0] = 'A' - 10 + temp;
	}
	convStr[2] = ' ';
	convStr[3] = '\0';
	KCEX_PrintString(convStr);
}

//【KCEX_Strlen】
// 测量一个字符串长度
// 参数：UBYTE* 字符串
DWORD KCEX_Strlen(UBYTE* str) 
{
	DWORD len = 0;
	while (*str != '\0') {
		len++;
		str++;
	}
	return len;
}

//----------------------------
//    功能性函数
//----------------------------

//【KCEX_IntToChar】
// 将数字转化为字符串
CHAR* KCEX_IntToChar(UDWORD num, CHAR* str, UBYTE base){
	CHAR base_map[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
	if (base > 16 || base <= 1)
		return str;
	DWORD b = 1, x = 0;
	while (num / (DWORD)base > 0) {
		b *= base;
		x++;
	}
	CHAR* i = str;
	while (x>=0) {
		(*i) = base_map[num / b];
		num = num % b;
		b /= base;
		x--;
		i++;
	}
	return str;
}

//【KCEX_PutChar】
// 打印字符
DWORD KCEX_PutChar(DWORD ch) {
	AF_DispChar(ch, CGA_DEFAULT_COLOR_W);
	return ch;
}