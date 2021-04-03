#pragma once
// ---------------------------------------------------------------			
// Kernel_CDef.h
// ---------------------------------------------------------------	
// Operating System Kernel Definition Header
// OS内核 - C-定义
// By Aeroraven, 2021-4-3
// --------------------------------------------------------------

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

typedef void VOID;

typedef char* CCSTRING; //字符串指针

typedef struct {
	UWORD limit_low;
	UWORD base_low;
	UBYTE base_mid;
	UBYTE attr_first;
	UBYTE limit_high_attr_sec;
	UBYTE base_high;
} DESCRIPTOR;

//常用常量
#define TRUE 1
#define FALSE 0
#define NULL 0
#define NULLPTR 0


//GDT和IDT常量
#define GDT_SIZE 128


//函数导入(函数定义在 Kernel_AsmFunc.asm中)

// 【AF_MemoryCopy】
// 内存复制
// 参数: ANYPTR destPtr 目标指针
// 参数: ANYPTR sourcePtr 来源指针
// 参数: UDWORD size 复制大小
// 返回：ANYPTR [eax] 目标指针
ANYPTR AF_MemoryCopy(ANYPTR destPtr, ANYPTR sourcePtr, UDWORD size);


// 【AF_LoadGlobalDescriptorTable】
// 加载GDT至GDTR寄存器
// 参数: GDTPTR gdtPtr  GDT指针(指针指向6字节内容)
// 返回：UDWORD [eax] 返回0 表示成功
UDWORD AF_LoadGlobalDescriptorTable(GDTPTR gdtPtr);


// 【AF_DispStrCGA】
// 将文字输出至CGA显存
// 参数: CCSTRING stringPtr  字符串指针
// 返回：UDWORD [eax] 返回0 表示成功
VOID AF_DispStrCGA(CCSTRING stringPtr);
