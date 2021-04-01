// ---------------------------------------------------------------			
// Kernel_GlobalVar.h
// ---------------------------------------------------------------	
// Operating System Kernel Global Variables
// OS内核 - 全局变量
// By Aeroraven, 2021-4-4
// --------------------------------------------------------------

#ifdef GLOBAL_VARS
#undef EXTERN
#define EXTERN
#endif

EXTERN UBYTE GDT_ptr[GDTR_SIZE];
EXTERN UBYTE IDT_ptr[IDTR_SIZE];

EXTERN UBYTE T[4];
EXTERN DESCRIPTOR GDT[GDT_SIZE];
EXTERN GATE IDT[IDT_SIZE];