#pragma once
// ---------------------------------------------------------------			
// Kernel_CDef.h
// ---------------------------------------------------------------	
// Operating System Kernel Definition Header
// OS�ں� - C-����
// By Aeroraven, 2021-4-3
// --------------------------------------------------------------

//���Ͷ���
typedef unsigned int UDWORD; 		//˫�֣���Ч��unsigned int
typedef unsigned short UWORD;	//�֣���Ч��unsigned short
typedef unsigned char UBYTE;		//�ֽڣ���Ч��unsigned char;

typedef int DWORD;		//˫�֣���Ч��int
typedef short WORD;		//�֣���Ч��short
typedef char  BYTE;		//�ֽڣ���Ч��char

typedef void* ANYPTR;	//������ָ��,��Ч��UDWORD
typedef void* GDTPTR;	//GDTָ��
typedef void* LDTPTR;	//LDTָ��

typedef void VOID;

typedef char* CCSTRING; //�ַ���ָ��

typedef struct {
	UWORD limit_low;
	UWORD base_low;
	UBYTE base_mid;
	UBYTE attr_first;
	UBYTE limit_high_attr_sec;
	UBYTE base_high;
} DESCRIPTOR;

//���ó���
#define TRUE 1
#define FALSE 0
#define NULL 0
#define NULLPTR 0


//GDT��IDT����
#define GDT_SIZE 128


//��������(���������� Kernel_AsmFunc.asm��)

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


// ��AF_DispStrCGA��
// �����������CGA�Դ�
// ����: CCSTRING stringPtr  �ַ���ָ��
// ���أ�UDWORD [eax] ����0 ��ʾ�ɹ�
VOID AF_DispStrCGA(CCSTRING stringPtr);
