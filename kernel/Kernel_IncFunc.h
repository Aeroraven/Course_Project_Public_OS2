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

// ��AF_SetDispPos��
// �������λ��
// ����: UDWORD dspPos ���λ�� 
// ���أ�VOID
UDWORD AF_SetDispPos(UDWORD dspPos);


//��AF_OutPort��
// �˿������Outָ��
// ������UWORD port �˿ں�
// ������UBYTE value ���ֵ
VOID AF_OutPort(UWORD port, UBYTE value);

//��AF_InPort��
// �˿����룬Inָ��
// ������UWORD port �˿ں�
UBYTE AF_InPort(UWORD port);

//��AF_UD2Call��
// ����UD2ָ��
VOID AF_UD2Call();


//----------------------------------------
//	�쳣�ж�(���������� Kernel_AsmFunc.asm��)
//----------------------------------------

VOID AFE_EXCEPTION_DE();
VOID AFE_EXCEPTION_DB();
VOID AFE_EXCEPTION_NMI();
VOID AFE_EXCEPTION_BP();
VOID AFE_EXCEPTION_OF();
VOID AFE_EXCEPTION_BR();
VOID AFE_EXCEPTION_UD();
VOID AFE_EXCEPTION_NM();
VOID AFE_EXCEPTION_DF();
VOID AFE_EXCEPTION_MF();
VOID AFE_EXCEPTION_TS();
VOID AFE_EXCEPTION_NP();
VOID AFE_EXCEPTION_SS();
VOID AFE_EXCEPTION_GP();
VOID AFE_EXCEPTION_PF();
VOID AFE_EXCEPTION_RSVD();
VOID AFE_EXCEPTION_MF2();
VOID AFE_EXCEPTION_AC();
VOID AFE_EXCEPTION_MC();
VOID AFE_EXCEPTION_XM();