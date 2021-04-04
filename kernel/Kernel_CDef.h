#pragma once
// ---------------------------------------------------------------			
// Kernel_CDef.h
// ---------------------------------------------------------------	
// Operating System Kernel Definition Header
// OS内核 - C-定义
// By Aeroraven, 2021-4-3
// --------------------------------------------------------------

//变量声明
#define EXTERN extern

//类型定义
typedef unsigned int UDWORD; 		//双字，等效于unsigned int
typedef unsigned short UWORD;	//字，等效于unsigned short
typedef unsigned char UBYTE;		//字节，等效于unsigned char;

typedef int DWORD;		//双字，等效于int
typedef short WORD;		//字，等效于short
typedef char  BYTE;		//字节，等效于char

typedef void* ANYPTR;	//空类型指针,等效于UDWORD
typedef void* GDTPTR;	//GDT指针
typedef void* LDTPTR;	//LDT指针
typedef void* IDTPTR;   //IDT指针

typedef void VOID;
typedef char CHAR;

typedef char* CCSTRING; //字符串指针
typedef void (*HANDLER)(); //函数指针

typedef char* VA_LIST; //可变参数列表

#define _INTSIZEOF(n) ((sizeof(n)+sizeof(DWORD)-1)&(~(sizeof(DWORD)-1)))
#define VA_START(ap, arg) (ap = (VA_LIST)&arg + _INTSIZEOF(arg))
#define VA_END(ap) (ap=(VA_LIST)0);
#define VA_ARG(ap, type) (*(type*)((ap+=_INTSIZEOF(type))-_INTSIZEOF(type)))

//GDT/IDT描述符
typedef struct DESCRIPTOR_s{
	UWORD limit_low;
	UWORD base_low;
	UBYTE base_mid;
	UBYTE attr_first;
	UBYTE limit_high_attr_sec;
	UBYTE base_high;
} DESCRIPTOR;			

//门描述符
typedef struct GATE_s{
	UWORD offset_low;
	UWORD selector;
	UBYTE dcount;
	UBYTE attr;
	UWORD offset_high;
} GATE;

//常用常量
#define TRUE 1
#define FALSE 0
#define NULL 0
#define NULLPTR 0
#define CONST const
#define PUBLIC
#define STATIC static
#define PRIVATE STATIC


//GDT和IDT常量
#define GDT_SIZE 128
#define GDTR_SIZE 6
#define IDT_SIZE 256
#define IDTR_SIZE 6

#define SELECTOR_KERNEL_CS 8

//CGA常量
#define CGA_DEFAULT_COLOR_W 0x3f //默认CGA颜色
#define CGA_WARNING_COLOR_W 0x0E //警告CGA颜色
#define CGA_EXCEPTION_COLOR_W 0x0C //异常CGA颜色

//其他
#define KCEX_PRINTF_BUFFERSIZE 1000