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

//GDT/IDT������
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
#define CONST const
#define PUBLIC
#define STATIC static
#define PRIVATE STATIC


//GDT��IDT����
#define GDT_SIZE 128
#define GDTR_SIZE 6


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

// ��AF_LoadGlobalDescriptorTable��
// ����GDT��GDTR�Ĵ���
// ����: GDTPTR gdtPtr  GDTָ��(ָ��ָ��6�ֽ�����)
// ���أ�UDWORD [eax] ����0 ��ʾ�ɹ�
UDWORD AF_LoadGlobalDescriptorTable(GDTPTR gdtPtr);

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