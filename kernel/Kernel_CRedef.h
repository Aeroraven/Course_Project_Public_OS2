#pragma once
// ---------------------------------------------------------------			
// Kernel_CRedef.h
// ---------------------------------------------------------------	
// Operating System Kernel C-Style Redefinition
// OS�ں� - CStyle�ض���
// By Aeroraven, 2021-4-4
// --------------------------------------------------------------

#include "Kernel_CDef.h"
#include "Kernel_CExt.h"

//C-Style

//cstdio
#define putchar KCEX_PutChar
#define printf KCEX_PrintFormat

//cstdlib
#define itoa KCEX_IntToChar

//cstring
#define memset KCEX_MemoryFill
#define strlen KCEX_Strlen
#define memcpy AF_MemoryCopy

//cmath
#define sin KCEX_Math_Sin
#define sqrt KCEX_Math_Sqrt
#define fabs KCEX_Math_Abs
#define cos KCEX_Math_Cos
#define tan KCEX_Math_Tan
#define exp KCEX_Math_Exp

//cstdarg
#define va_list VA_LIST
#define va_start VA_START
#define va_end VA_END
#define va_arg VA_ARG


//cpp-algorithm
#define max(a,b) ((a>b)?(a):(b))
#define min(a,b) ((a<b)?(a):(b))


//Assembly Instructions

#define __sgdt AF_SaveGlobalDescriptorTable //(ASM) SGDT ָ��
#define __lgdt AF_LoadGlobalDescriptorTable //(ASM) LGDT ָ��
#define __lidt AF_LoadInterruptDescriptorTable
#define __out AF_OutPort
#define __in AF_InPort
#define __ud2 AF_UD2Call
#define __sti AF_STICall //(ASM) STI ָ��
#define __cli AF_CLICall //(ASM) CLI ָ��
#define __hlt AF_HLTCall //(ASM) HLT ָ��
#define __ltr AF_LTRAxCall //(ASM) LTR ָ��

#define _cli asm("cli");
#define _sti asm("sti");
