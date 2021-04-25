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
#include "Kernel_GlobalVar.h"

//----------------------------
//    输出设置
//----------------------------

CONST DWORD CGA_COLMAX = 80; //CGA最大行
CONST UBYTE CGA_DEFAULT_COLOR = 0x3f;


VOID KCEX_PrintChar(UBYTE character);
VOID KCEX_PrintString(UBYTE* chString);
VOID KCEX_PrintByteHex(UBYTE arg);
DWORD KCEX_Strlen(UBYTE* str);
CHAR* KCEX_IntToChar(DWORD num, CHAR* str, UBYTE base);
DWORD KCEX_PutChar(DWORD ch);
DWORD KCEX_MemoryFill(CHAR* dest, UBYTE ch, UDWORD size);
DWORD KCEX_PrintFormat(CHAR* format, ...);

VOID KC_IDT_LoadGate(UDWORD vector, UBYTE desc_type, HANDLER handler, UBYTE privilege);

//【KCEX_PrintChar】
// 打印字符
// 参数：UBYTE character 要打印的字符
VOID KCEX_PrintChar(UBYTE character) {
	DWORD CGA_CURRENTCOL = 0; //CGA当前列
	if (character != '\n') {
		KC_VESA_PutChar(character, KRNL_VESAFont_Row, KRNL_VESAFont_Col, 0xff, 0xff, 0xff);
		CGA_CURRENTCOL = KRNL_VESAFont_Col;
		KRNL_VESAFont_Col++;
		if (CGA_CURRENTCOL >= VESA_FONT_COLMAX) {
			CGA_CURRENTCOL = 0;
			KRNL_VESAFont_Row = KRNL_VESAFont_Row + 1;
			KRNL_VESAFont_Col = 0;
		}
		return;
	}
	KRNL_VESAFont_Row++;
	CGA_CURRENTCOL = 0;
	KRNL_VESAFont_Col = 0;
}

VOID KCEX_PrintChar_Except(UBYTE character) {
	DWORD CGA_CURRENTCOL = 0; //CGA当前列
	if (character != '\n') {
		KC_VESA_PutChar(character, KRNL_VESAFont_Row, KRNL_VESAFont_Col, 0xff, 0, 0);
		CGA_CURRENTCOL = KRNL_VESAFont_Col;
		KRNL_VESAFont_Col++;
		if (CGA_CURRENTCOL >= VESA_FONT_COLMAX) {
			CGA_CURRENTCOL = 0;
			KRNL_VESAFont_Row = KRNL_VESAFont_Row + 1;
			KRNL_VESAFont_Col = 0;
		}
		return;
	}
	KRNL_VESAFont_Row++;
	CGA_CURRENTCOL = 0;
	KRNL_VESAFont_Col = 0;
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
VOID KCEX_PrintStringE(UBYTE* chString) {
	UBYTE* i = chString;
	while (*i != '\0') {
		KCEX_PrintChar_Except(*i);
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
DWORD KCEX_Strlen(UBYTE* str) {
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
CHAR* KCEX_IntToChar(DWORD num, CHAR* str, UBYTE base){
	CHAR base_map[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
	DWORD nflag = 0;
	if (num < 0) {
		num = -num;
		nflag = 1;
	}
	if (base > 16 || base <= 1)
		return str;
	DWORD b = 1, x = 0;
	while (num / (UDWORD)b > 0) {
		b *= base;
		x++;
	}
	if (x > 0) {
		b /= base;
		x--;
	}
	CHAR* i = str;
	if (nflag) {
		*i = '-';
		i++;
	}
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
	KCEX_PrintChar(ch);
	return ch;
}

DWORD KCEX_MemoryFill(CHAR* dest, UBYTE ch, UDWORD size) {
	CHAR* s = dest;
	for (UDWORD i = 0; i < size; i++) {
		(*s) = ch;
		s++;
	}
}

//【KCEX_PrintFormat】(Deprecated)
// 格式化输出(printf)
DWORD KCEX_PrintFormat(CHAR* format, ...) {
	VA_LIST argument_list;
	CHAR* format_iter = format;
	VA_START(argument_list, format);

	DWORD t;
	CHAR t2;
	CHAR* t3;
	CHAR buffer[KCEX_PRINTF_BUFFERSIZE];

	while ((*format_iter) != '\0') {
		if ((*format_iter) == '%') {
			format_iter++;
			switch ((*format_iter)) {
				case 'd': //DEC int
					t = VA_ARG(argument_list, DWORD);
					KCEX_MemoryFill(buffer, 0, KCEX_PRINTF_BUFFERSIZE);
					KCEX_IntToChar(t, buffer, 10);
					KCEX_PrintString(buffer);
					break;
				case 'i': //DEC int
					t = VA_ARG(argument_list, DWORD);
					KCEX_MemoryFill(buffer, 0, KCEX_PRINTF_BUFFERSIZE);
					KCEX_IntToChar(t, buffer, 10);
					KCEX_PrintString(buffer);
					break;
				case 'o': //DEC oct
					t = VA_ARG(argument_list, DWORD);
					KCEX_MemoryFill(buffer, 0, KCEX_PRINTF_BUFFERSIZE);
					KCEX_IntToChar(t, buffer, 8);
					KCEX_PrintString(buffer);
					break;
				case 'x': //DEC hex
					t = VA_ARG(argument_list, DWORD);
					KCEX_MemoryFill(buffer, 0, KCEX_PRINTF_BUFFERSIZE);
					KCEX_IntToChar(t, buffer, 16);
					KCEX_PrintString(buffer);
					break;
				case 'X': //DEC hex
					t = VA_ARG(argument_list, DWORD);
					KCEX_MemoryFill(buffer, 0, KCEX_PRINTF_BUFFERSIZE);
					KCEX_IntToChar(t, buffer, 16);
					KCEX_PrintString(buffer);
					break;
				case 'c':
					t2 = VA_ARG(argument_list, CHAR);
					KCEX_PutChar(t2);
					break;
				case 's':
					t3 = VA_ARG(argument_list,CHAR*);
					KCEX_PrintString(t3);
					break;
				
			}
		}
		else {
			KCEX_PrintChar(*format_iter);
			
			
		}
		format_iter++;
	}

}



//----------------------------
//    数学相关
//----------------------------
DOUBLE KCEX_Math_Sin(DOUBLE x) {
	DWORD fact = 1;
	DOUBLE ans = 0;
	DOUBLE xs = 1;
	for (DWORD i = 1, j = 1; i < KCEX_MATH_TAYLOR_LIM; i += 2, j = -j) {
		xs *= x;
		fact *= i;
		ans += xs * j / fact;
		xs *= x;
		fact *= (i + 1);
	}
	return ans;
}
DOUBLE KCEX_Math_Cos(DOUBLE x) {
	DWORD fact = 1;
	DOUBLE ans = 0;
	DOUBLE xs = 1;
	for (DWORD i = 1, j = 1; i < KCEX_MATH_TAYLOR_LIM; i += 2, j = -j) {
		ans += xs * j / fact;
		xs *= x;
		xs *= x;
		fact *= i;
		fact *= (i + 1);
	}
	return ans;
}
DOUBLE KCEX_Math_Abs(DOUBLE x) {
	if (x < 0.0)x = -x;
	return x;
}
DOUBLE KCEX_Math_Sqrt(DOUBLE x) {
	DOUBLE L = 0.0, R = x;
	while (KCEX_Math_Abs(R - L) > KCEX_MATH_DBL_EPS) {
		DOUBLE m = (L + R) / 2;
		if (m*m < x) {
			L = m;
		}
		else {
			R = m;
		}
	}
	return L;
}
DOUBLE KCEX_Math_Tan(DOUBLE x) {
	return KCEX_Math_Sin(x) / KCEX_Math_Cos(x);
}
DOUBLE KCEX_Math_ExpDecTaylor(DOUBLE x) {
	DOUBLE frac = 1;
	DOUBLE sx = 1;
	DOUBLE ans = 0;
	for (DWORD i = 0; i < KCEX_MATH_TAYLOR_LIM; i++) {
		ans += sx / frac;
		sx *= x;
		frac *= (double)(i + 1);
	}
	return ans;
}
DOUBLE KCEX_Math_ExpIntPow(UDWORD x) {
	DOUBLE base = 1;
	DOUBLE ans = 1;
	UDWORD dx = x;
	while (dx) {
		if (dx & 1) ans = ans * (double)base;
		dx >>= 1;
		base = base * base;
	}
	return ans;
}
DOUBLE KCEX_Math_Exp(DOUBLE x) {
	UDWORD intp = (DWORD)x;
	DOUBLE decp = x - (DOUBLE)intp;
	return KCEX_Math_ExpDecTaylor(decp) + KCEX_Math_ExpIntPow(intp);
}