#pragma once
// ---------------------------------------------------------------			
// Kernel_CExt.h
// ---------------------------------------------------------------	
// Operating System Kernel Extension Header
// OS�ں� - ������չ (������TAT)
// By Aeroraven, 2021-4-3
// --------------------------------------------------------------

#include "Kernel_CDef.h"

//�������
CONST DWORD CGA_COLMAX = 80; //CGA�����
CONST UBYTE CGA_DEFAULT_COLOR = 0x3f;

//��KCEX_PrintChar��
// ��ӡ�ַ�
// ������UBYTE character Ҫ��ӡ���ַ�
VOID KCEX_PrintChar(UBYTE character) {
	DWORD CGA_CURRENTCOL = 0; //CGA��ǰ��
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

//��KCEX_PrintString��
// ��ӡ�ַ�
// ������UBYTE* chString Ҫ��ӡ���ַ���
VOID KCEX_PrintString(UBYTE* chString) {
	UBYTE* i = chString;
	while (*i != '\0') {
		KCEX_PrintChar(*i);
		i++;
	}
}

//��KCEX_PrintByteHex��
// ��ӡһ���ֽ�(ʮ������)
// ������UBYTE arg Ҫ��ӡ���ֽ�
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