#pragma once
// ---------------------------------------------------------------			
// Kernel_CRedef.h
// ---------------------------------------------------------------	
// Operating System Kernel C-Style Redefinition
// OS内核 - CStyle重定义
// By Aeroraven, 2021-4-4
// --------------------------------------------------------------

#include "Kernel_CDef.h"
#include "Kernel_CExt.h"

//C-Style
#define strlen KCEX_Strlen
#define memcpy AF_MemoryCopy
#define itoa KCEX_IntToChar
#define putchar KCEX_PutChar
#define printf KCEX_PrintFormat
#define memset KCEX_MemoryFill
#define max(a,b) ((a>b)?(a):(b))
#define min(a,b) ((a<b)?(a):(b))


//Assembly Instructions

#define _sgdt AF_SaveGlobalDescriptorTable //SGDT 指令
#define _lgdt AF_LoadGlobalDescriptorTable
#define _lidt AF_LoadInterruptDescriptorTable
#define _out AF_OutPort
#define _in AF_InPort