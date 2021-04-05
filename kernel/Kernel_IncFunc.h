#pragma once

// ---------------------------------------------------------------			
// Kernel_IncFunc.h
// ---------------------------------------------------------------	
// Operating System Kernel Function Inclusion
// OS内核 - 函数引入
// By Aeroraven, 2021-4-4
// --------------------------------------------------------------

//----------------------------------------
//	函数导入(函数定义在 Kernel_AsmFunc.asm中)
//----------------------------------------
#include "Kernel_CDef.h"


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

// 【AF_LoadGlobalDescriptorTable】
// 加载GDT至GDTR寄存器
// 参数: GDTPTR gdtPtr  GDT指针(指针指向6字节内容)
// 返回：UDWORD [eax] 返回0 表示成功
UDWORD AF_LoadGlobalDescriptorTable(GDTPTR gdtPtr);

// 【AF_LoadInterrputDescriptorTable】
// 加载IDT至IDTR寄存器
// 参数: IDTPTR gdtPtr  IDT指针(指针指向6字节内容)
// 返回：UDWORD [eax] 返回0 表示成功
UDWORD AF_LoadInterruptDescriptorTable(IDTPTR gdtPtr);


// 【AF_SaveGlobalDescriptorTable】
// 保存GDTR寄存器至内存
// 参数: GDTPTR gdtPtr  GDT指针(指针指向6字节内容)
// 返回：UDWORD [eax] 返回0 表示成功
UDWORD AF_SaveGlobalDescriptorTable(GDTPTR gdtPtr);


// 【AF_VMBreakPoint】
// 虚拟机断点，用于产生Bochs断点
VOID AF_VMBreakPoint();


// 【AF_DispChar】
// 将文字输出至CGA显存
// 参数: UDWORD ch 字符,请保证在UBYTE范围内
//		 UDWORD style 样式，请保证在UBYTE范围内
// 返回：VOID
VOID AF_DispChar(UDWORD ch, UDWORD style);


// 【AF_GetDispPos】
// 获取输出位置
// 参数: UDWORD* dspPos 接受返回值的变量 
// 返回：VOID
UDWORD AF_GetDispPos(UDWORD* dspPos);

// 【AF_SetDispPos】
// 设置输出位置
// 参数: UDWORD dspPos 输出位置 
// 返回：VOID
UDWORD AF_SetDispPos(UDWORD dspPos);


//【AF_OutPort】
// 端口输出，Out指令
// 参数：UWORD port 端口号
// 参数：UBYTE value 输出值
VOID AF_OutPort(UWORD port, UBYTE value);

//【AF_InPort】
// 端口输入，In指令
// 参数：UWORD port 端口号
UBYTE AF_InPort(UWORD port);

//【AF_UD2Call】
// 调用UD2指令
VOID AF_UD2Call();


//----------------------------------------
//	异常中断(函数定义在 Kernel_AsmFunc.asm中)
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