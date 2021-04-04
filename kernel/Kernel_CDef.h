#pragma once
// ---------------------------------------------------------------			
// Kernel_CDef.h
// ---------------------------------------------------------------	
// Operating System Kernel Definition Header
// OS�ں� - C-����
// By Aeroraven, 2021-4-3
// --------------------------------------------------------------

//��������
#define EXTERN extern

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
typedef void* IDTPTR;   //IDTָ��

typedef void VOID;
typedef char CHAR;

typedef char* CCSTRING; //�ַ���ָ��
typedef void (*HANDLER)(); //����ָ��

typedef char* VA_LIST; //�ɱ�����б�

#define _INTSIZEOF(n) ((sizeof(n)+sizeof(DWORD)-1)&(~(sizeof(DWORD)-1)))
#define VA_START(ap, arg) (ap = (VA_LIST)&arg + _INTSIZEOF(arg))
#define VA_END(ap) (ap=(VA_LIST)0);
#define VA_ARG(ap, type) (*(type*)((ap+=_INTSIZEOF(type))-_INTSIZEOF(type)))

//GDT/IDT������
typedef struct DESCRIPTOR_s{
	UWORD limit_low;
	UWORD base_low;
	UBYTE base_mid;
	UBYTE attr_first;
	UBYTE limit_high_attr_sec;
	UBYTE base_high;
} DESCRIPTOR;			

//��������
typedef struct GATE_s{
	UWORD offset_low;
	UWORD selector;
	UBYTE dcount;
	UBYTE attr;
	UWORD offset_high;
} GATE;

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
#define IDT_SIZE 256
#define IDTR_SIZE 6

#define SELECTOR_KERNEL_CS 8

//CGA����
#define CGA_DEFAULT_COLOR_W 0x3f //Ĭ��CGA��ɫ
#define CGA_WARNING_COLOR_W 0x0E //����CGA��ɫ
#define CGA_EXCEPTION_COLOR_W 0x0C //�쳣CGA��ɫ

//����
#define KCEX_PRINTF_BUFFERSIZE 1000