// ---------------------------------------------------------------			
// Kernel_C.c
// ---------------------------------------------------------------	
// Operating System Kernel 
// OSÄÚºË
// By Aeroraven, 2021-4-3
// --------------------------------------------------------------
#include "Kernel_CDef.h"

UBYTE old_GDT[6];
DESCRIPTOR GDT[GDT_SIZE];

VOID kernel_start() {
	AF_DispStrCGA("Hello World");
}