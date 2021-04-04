#pragma once

// ---------------------------------------------------------------			
// Kernel_IncFunc.h
// ---------------------------------------------------------------	
// Operating System Kernel Function Inclusion
// OS�ں� - ��������
// By Aeroraven, 2021-4-4
// --------------------------------------------------------------

//----------------------------------------
//	��������(���������� Kernel_AsmFunc.asm��)
//----------------------------------------
#include "Kernel_CDef.h"
// ��AF_MemoryCopy��
// �ڴ渴��
// ����: ANYPTR destPtr Ŀ��ָ��
// ����: ANYPTR sourcePtr ��Դָ��
// ����: UDWORD size ���ƴ�С
// ���أ�ANYPTR [eax] Ŀ��ָ��
ANYPTR AF_MemoryCopy(ANYPTR destPtr, ANYPTR sourcePtr, UDWORD size);


// ��AF_LoadGlobalDescriptorTable��
// ����GDT��GDTR�Ĵ���
// ����: GDTPTR gdtPtr  GDTָ��(ָ��ָ��6�ֽ�����)
// ���أ�UDWORD [eax] ����0 ��ʾ�ɹ�
UDWORD AF_LoadGlobalDescriptorTable(GDTPTR gdtPtr);

// ��AF_LoadGlobalDescriptorTable��
// ����GDT��GDTR�Ĵ���
// ����: GDTPTR gdtPtr  GDTָ��(ָ��ָ��6�ֽ�����)
// ���أ�UDWORD [eax] ����0 ��ʾ�ɹ�
UDWORD AF_LoadGlobalDescriptorTable(GDTPTR gdtPtr);

// ��AF_LoadInterrputDescriptorTable��
// ����IDT��IDTR�Ĵ���
// ����: IDTPTR gdtPtr  IDTָ��(ָ��ָ��6�ֽ�����)
// ���أ�UDWORD [eax] ����0 ��ʾ�ɹ�
UDWORD AF_LoadInterruptDescriptorTable(IDTPTR gdtPtr);


// ��AF_SaveGlobalDescriptorTable��
// ����GDTR�Ĵ������ڴ�
// ����: GDTPTR gdtPtr  GDTָ��(ָ��ָ��6�ֽ�����)
// ���أ�UDWORD [eax] ����0 ��ʾ�ɹ�
UDWORD AF_SaveGlobalDescriptorTable(GDTPTR gdtPtr);


// ��AF_VMBreakPoint��
// ������ϵ㣬���ڲ���Bochs�ϵ�
VOID AF_VMBreakPoint();


// ��AF_DispChar��
// �����������CGA�Դ�
// ����: UDWORD ch �ַ�,�뱣֤��UBYTE��Χ��
//		 UDWORD style ��ʽ���뱣֤��UBYTE��Χ��
// ���أ�VOID
VOID AF_DispChar(UDWORD ch, UDWORD style);


// ��AF_GetDispPos��
// ��ȡ���λ��
// ����: UDWORD* dspPos ���ܷ���ֵ�ı��� 
// ���أ�VOID
UDWORD AF_GetDispPos(UDWORD* dspPos);

//��AF_OutPort��
// �˿������Outָ��
// ������UWORD port �˿ں�
// ������UBYTE value ���ֵ
VOID AF_OutPort(UWORD port, UBYTE value);

//��AF_InPort��
// �˿����룬Inָ��
// ������UWORD port �˿ں�
UBYTE AF_InPort(UWORD port);