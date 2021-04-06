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

//内核开始
VOID kernel_main() {
	AF_STICall();
	AF_HLTCall();
}

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
	
	initalize_interrupts();

	//开始处理
	kernel_main();
}

//异常处理
VOID exception_handler(DWORD int_vector_no, DWORD error_code, DWORD eip, DWORD cs, DWORD eflag) {
	AF_SetDispPos(0);
	for (int i = 0; i < 25; i++) {
		KCEX_PrintStringE("\n");
	}
	AF_SetDispPos(0);
	KCEX_PrintStringE("Exception Detected!\n");
	CHAR buffer[2000];
	
	KCEX_PrintStringE("----------------------\n");
	KCEX_PrintStringE("Info:(");
	KCEX_PrintStringE(KCEX_IntToChar(int_vector_no, buffer, 10));
	KCEX_PrintStringE(")");
	switch (int_vector_no) {
		case 0:
			KCEX_PrintStringE(KRNL_INTTIPS_DE);
			break;
		case 13:
			KCEX_PrintStringE(KRNL_INTTIPS_SS);
			break;
		default:
			KCEX_PrintStringE(KRNL_INTTIPS_UNKNOWN);
			break;
	}
	KCEX_PrintStringE("\n\n");

	KCEX_PrintStringE("ErrCode:");
	KCEX_PrintStringE(KCEX_IntToChar(error_code, buffer, 16));
	KCEX_PrintStringE("\n\n");

	KCEX_PrintStringE("IPReg:");
	KCEX_PrintStringE(KCEX_IntToChar(eip, buffer, 16));
	KCEX_PrintStringE("\n\n");
	
	KCEX_PrintStringE("CodeSeg:");
	KCEX_PrintStringE(KCEX_IntToChar(cs, buffer, 16));
	KCEX_PrintStringE("\n\n");
	
	KCEX_PrintStringE("Flag:");
	KCEX_PrintStringE(KCEX_IntToChar(eflag, buffer, 16));
	KCEX_PrintStringE("\n\n");
	return;
}


//8259A初始化
VOID initialize_8259A() {
	
	AF_OutPort(KRNL_INT_M_CTL, 0x11);
	AF_OutPort(KRNL_INT_S_CTL, 0x11);
	AF_OutPort(KRNL_INT_M_CTLMASK, KRNL_INT_VECTOR_IRQ0);
	AF_OutPort(KRNL_INT_S_CTLMASK, KRNL_INT_VECTOR_IRQ8);
	AF_OutPort(KRNL_INT_M_CTLMASK, 0x4);
	AF_OutPort(KRNL_INT_S_CTLMASK, 0x2);
	AF_OutPort(KRNL_INT_M_CTLMASK, 0x1);
	AF_OutPort(KRNL_INT_S_CTLMASK, 0x1);
	AF_OutPort(KRNL_INT_M_CTLMASK, 0xFF);
	AF_OutPort(KRNL_INT_S_CTLMASK, 0xFF);

	//开启键盘中断
	AF_OutPort(KRNL_INT_M_CTLMASK, 0xFD);
	AF_OutPort(KRNL_INT_S_CTLMASK, 0xFF);

}

//伪造中断请求
VOID spurious_interrupt_request(UDWORD idx) {
	CHAR buffer[12];
	KCEX_PrintString("Spurious Int Handler:");
	KCEX_PrintString(KCEX_IntToChar(idx, buffer, 10));
}

//初始化中断
VOID initalize_interrupts() {
	initialize_8259A();
	//异常
	KC_IDT_LoadGate(KRNL_INT_VECTOR_DIVIDE, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_DE, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_DEBUG, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_DB, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_NMI, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_NMI, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_BREAKPOINT, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_BP, KRNL_PRIVL_USR);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_OVERFLOW, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_OF, KRNL_PRIVL_USR);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_BOUNDS, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_BR, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_INVAL_OP, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_UD, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_COPROC_NOT, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_NM, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_DOUBLE_FAULT, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_DF, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_COPROC_SEG, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_MF, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_INVAL_TSS, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_TS, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_SEG_NOT, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_NP, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_STACK_FAULT, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_SS, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_PROTECTION, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_GP, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_PAGE_FAULT, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_PF, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_COPROC_ERR, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_EXCEPTION_MF2, KRNL_PRIVL_SYS);

	//硬件中断
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ0 + 0, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_0, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ0 + 1, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_1, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ0 + 2, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_2, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ0 + 3, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_3, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ0 + 4, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_4, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ0 + 5, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_5, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ0 + 6, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_6, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ0 + 7, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_7, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ8 + 0, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_8, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ8 + 1, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_9, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ8 + 2, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_10, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ8 + 3, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_11, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ8 + 4, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_12, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ8 + 5, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_13, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ8 + 6, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_14, KRNL_PRIVL_SYS);
	KC_IDT_LoadGate(KRNL_INT_VECTOR_IRQ8 + 7, KRNL_DESCRIPTOR_ATTR_386IGate, AFE_INT_15, KRNL_PRIVL_SYS);
}

