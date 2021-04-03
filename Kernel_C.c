// ---------------------------------------------------------------			
// Kernel_C.c
// ---------------------------------------------------------------	
// Operating System Kernel 
// OSÄÚºË
// By Aeroraven, 2021-4-3
// --------------------------------------------------------------
#include "Kernel_CDef.h"
#include "Kernel_CExt.h"

UBYTE old_GDT[GDTR_SIZE] = { 0,0,0,0,0,0 };
UBYTE T[4] = { 'a','b','c','\0' };
DESCRIPTOR GDT[GDT_SIZE];


VOID kernel_start() {
	KCEX_PrintString("HelloWorld====\n");
	KCEX_PrintString("I am shabi\n");
	KCEX_PrintString("I am a big shabi\n");
	KCEX_PrintString("I am a super shabi\n");
	for (DWORD i = 0; i < 6; i++) {
		KCEX_PrintByteHex(old_GDT[i]);
	}
	
	KCEX_PrintString("\n");
	AF_SaveGlobalDescriptorTable((GDTPTR)old_GDT);
	AF_MemoryCopy(&GDT, (ANYPTR)(*((UDWORD*)(&old_GDT[2]))), *((UWORD*)(&old_GDT[0]) + 1));
	for (DWORD i = 0; i < GDTR_SIZE; i++) {
		KCEX_PrintByteHex(old_GDT[i]);
	}
	KCEX_PrintString("\n");
	KCEX_PrintString("sizeof(WORD)");
	KCEX_PrintByteHex(sizeof(WORD));
	KCEX_PrintString("\n");

	KCEX_PrintString("sizeof(DWORD)");
	KCEX_PrintByteHex(sizeof(DWORD));
	KCEX_PrintString("\n");

	KCEX_PrintString("sizeof(BYTE)");
	KCEX_PrintByteHex(sizeof(BYTE));
	KCEX_PrintString("\n");
}