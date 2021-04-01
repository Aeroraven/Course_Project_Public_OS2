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

//初始化内核
VOID kernel_start() {

	printf("%s%x", "His birthday is ", 19260817);
	
	//复制GDT
	AF_SaveGlobalDescriptorTable((GDTPTR)GDT_ptr);
	AF_MemoryCopy(&GDT, (ANYPTR)(*((UDWORD*)(&GDT_ptr[2]))), *((UWORD*)(&GDT_ptr[0]) + 1));

	UWORD* t_gdtLimit = (UWORD*)(&GDT_ptr[0]);
	UDWORD* t_gdtBase = (UDWORD*)(&GDT_ptr[2]);
	*t_gdtLimit = IDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*t_gdtBase = (UDWORD)&GDT;
	AF_LoadGlobalDescriptorTable(GDT_ptr);
	
	//处理IDT
	UWORD* t_idtLimit = (UWORD*)(&IDT_ptr[0]);
	UDWORD* t_idtBase = (UDWORD*)(&IDT_ptr[2]);
	*t_idtLimit = IDT_SIZE * sizeof(GATE) - 1;
	*t_idtBase = (UDWORD)&IDT;
	AF_LoadInterruptDescriptorTable(IDT_ptr);
	
}

//异常处理
VOID exception_handler(DWORD int_vector_no, DWORD error_code, DWORD eip, DWORD cs, DWORD eflag) {
	KCEX_PrintString("Exception");
}


VOID initialize_i8259() {

}