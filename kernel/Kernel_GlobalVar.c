// ---------------------------------------------------------------			
// Kernel_GlobalVar.c
// ---------------------------------------------------------------	
// Operating System Kernel Global Variables
// OS内核 - 全局变量
// By Aeroraven, 2021-4-4
// --------------------------------------------------------------

#define GLOBAL_VARS 1

#include "Kernel_CDef.h"
#include "Kernel_GlobalVar.h"

PROCESS ProcessTable[KRNL_PROC_MAXCNT];

CHAR TestStack[8000];
CHAR TestStack2[8000];
CHAR TestStack3[8000];

TSS tss;
PROCESS* ProcessReady;
DWORD TickCount;

TASK task_table[KRNL_PROC_MAXCNT];

IRQ_HANDLER irq_handler[KRNL_INT_IRQ_COUNTS];
SYSCALL syscall_table[KRNL_SYSCALL_COUNTS];