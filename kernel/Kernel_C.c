// ---------------------------------------------------------------			
// Kernel_C.c
// ---------------------------------------------------------------	
// Operating System Kernel 
// OS内核
// By Aeroraven, 2021-4-3
// --------------------------------------------------------------
#include "Kernel_CDef.h"
#include "Kernel_CExt.h"
#include "Kernel_CRedef.h"
#include "Kernel_GlobalVar.h"

VOID kernel_start() {
	KCEX_PrintString("HelloWorld====\n");
	KCEX_PrintString("I am shabi\n");
	KCEX_PrintString("I am a big shabi\n");
	KCEX_PrintString("I am a super shabi\n");
	//复制GDT
	AF_SaveGlobalDescriptorTable((GDTPTR)GDT_ptr);
	AF_MemoryCopy(&GDT, (ANYPTR)(*((UDWORD*)(&GDT_ptr[2]))), *((UWORD*)(&GDT_ptr[0]) + 1));

	//处理IDT
	UWORD* t_idtLimit = (UWORD*)(&IDT_ptr[0]);
	UDWORD* t_idtBase = (UDWORD*)(&IDT_ptr[2]);
	*t_idtLimit = IDT_SIZE * sizeof(GATE) - 1;
	*t_idtBase = (UDWORD)&IDT;
}