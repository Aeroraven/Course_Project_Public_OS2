#pragma once
// ---------------------------------------------------------------			
// Kernel_CCore.h
// ---------------------------------------------------------------	
// Operating System Kernel Core Functions
// OS内核 - 重要函数
// By Aeroraven, 2021-4-7
// --------------------------------------------------------------

#include "Kernel_CDef.h"
#include "Kernel_IncFunc.h"
#include "Kernel_GlobalVar.h"

//----------------------------
//    中断处理
//----------------------------

VOID KC_IDT_LoadGate(UDWORD vector, UBYTE desc_type, HANDLER handler, UBYTE privilege) {
	GATE* p_gate = &IDT[vector];
	UDWORD	base = (UDWORD)handler;
	p_gate->offset_low = base & 0xFFFF;
	p_gate->selector = SELECTOR_KERNEL_CS;
	p_gate->dcount = 0;
	p_gate->attr = desc_type | (privilege << 5);
	p_gate->offset_high = (base >> 16) & 0xFFFF;
}

VOID KC_IRQ_Disable(UDWORD irq_id) {
	if (irq_id < 8) {
		AF_OutPort(KRNL_INT_M_CTLMASK, AF_InPort(KRNL_INT_M_CTLMASK) | (1 << irq_id));
	}
	else {
		AF_OutPort(KRNL_INT_S_CTLMASK, AF_InPort(KRNL_INT_S_CTLMASK) | (1 << irq_id));
	}
}

VOID KC_IRQ_Enable(UDWORD irq_id) {
	if (irq_id < 8) {
		AF_OutPort(KRNL_INT_M_CTLMASK, AF_InPort(KRNL_INT_M_CTLMASK) &~ (1 << irq_id));
	}
	else {
		AF_OutPort(KRNL_INT_S_CTLMASK, AF_InPort(KRNL_INT_S_CTLMASK) &~ (1 << irq_id));
	}
}

VOID KC_IRQ_Establish(UDWORD irq_id, IRQ_HANDLER handler) {
	KC_IRQ_Disable(irq_id);
	irq_handler[irq_id] = handler;
}
//----------------------------
//    描述符处理
//----------------------------
VOID KC_LoadDescriptor(DESCRIPTOR* p_desc, UDWORD base, UDWORD limit, UWORD attribute)
{
	p_desc->limit_low = limit & 0x0FFFF;
	p_desc->base_low = base & 0x0FFFF;
	p_desc->base_mid = (base >> 16) & 0x0FF;
	p_desc->attr_first = attribute & 0xFF;
	p_desc->limit_high_attr_sec = ((limit >> 16) & 0x0F) | (attribute >> 8) & 0xF0;
	p_desc->base_high = (base >> 24) & 0x0FF;
}

VOID KC_LoadProcessInfo(PROCESS* proc,SELECTOR_W ldt_selector, SELECTOR_S cs, SELECTOR_S ds, SELECTOR_S es, SELECTOR_S fs,
	SELECTOR_S gs, SELECTOR_S ss, HANDLER proc_entry, DWORD stack_address, DWORD eflag) {
	//AF_VMBreakPoint();
	proc->ldt_selector = ldt_selector;
	proc->regs.cs = KRNL_LDSREG_S(cs);
	proc->regs.ds = KRNL_LDSREG_S(ds);
	proc->regs.es = KRNL_LDSREG_S(es);
	proc->regs.fs = KRNL_LDSREG_S(fs);
	proc->regs.gs = KRNL_LDSREG_S(gs);
	proc->regs.ss = KRNL_LDSREG_S(ss);
	proc->regs.eip = (UDWORD)proc_entry;
	proc->regs.esp = stack_address;
	proc->regs.eflags = eflag;
	//AF_VMBreakPoint();
}
VOID KC_DuplicateDescriptor(DESCRIPTOR* dest, DESCRIPTOR* src) {
	AF_MemoryCopy(dest, src, sizeof(DESCRIPTOR));
}
VOID KC_DuplicateGlobalDescriptor(DESCRIPTOR* dest, SELECTOR_W gdt_selector) {
	KC_DuplicateDescriptor(dest, &GDT[gdt_selector >> 3]);
}
VOID KC_DuplicateGlobalDescriptorEx(DESCRIPTOR* dest, SELECTOR_W gdt_selector,UDWORD attr1) {
	KC_DuplicateGlobalDescriptor(dest, gdt_selector);
	dest->attr_first = attr1;
}

UDWORD KC_GetPhyAddrBySeg(UWORD seg)
{
	DESCRIPTOR* p_dest = &GDT[seg >> 3];
	return (p_dest->base_high << 24 | p_dest->base_mid << 16 | p_dest->base_low);
}

VOID KC_InitTSS() {
	CHAR* s = (CHAR*)&tss;
	for (UDWORD i = 0; i < sizeof(TSS); i++) {
		(*s) = 0;
		s++;
	}
	tss.ss0 = KRNL_LSELECTOR_GENERALDATA;
	KC_LoadDescriptor(&GDT[KRNL_LSELECTOR_TSS >> 3], KC_GetPhyAddrBySeg(KRNL_LSELECTOR_GENERALDATA) + (UDWORD)(&tss), sizeof(TSS) - 1, KRNL_DESCRIPTOR_ATTR_386TSS);
	tss.iobase = sizeof(tss);
	AF_LTRAxCall(KRNL_LSELECTOR_TSS);
}

//----------------------------
//    系统调用
//----------------------------
DWORD KCHD_SysCall_GetTick() {
	return K_Ticks;
}
VOID KC_SysCall_Establish(UDWORD id, VOID* handler) {
	syscall_table[id] = handler;
}
//----------------------------
//    进程和作业
//----------------------------
VOID KC_LoadTaskTable(UWORD id, HANDLER handler, DWORD stacksize, CHAR* taskname, UBYTE* stack_addr,DWORD priority) {
	for (CHAR* i = taskname, *j = task_table[id].name;;)
	{
		*j = *i;
		if (*i == '\0')
		{
			break;
		}
		i++;
		j++;
	}
	task_table[id].stack_size = stacksize;
	task_table[id].task_eip = handler;
	task_table[id].stack_ptr = stack_addr;
	task_table[id].priority = priority;
}

VOID KC_ProcessSchedule() {
	ProcessReady->remaining_ticks--;
	PROCESS* proc;
	PROCESS* proc_cur = ProcessReady;
	DWORD remaining_tick_max = 0;
	if (proc_cur->remaining_ticks == 0) {
		while (!remaining_tick_max) {
			for (DWORD i = 0; i < KRNL_PROC_MAXTASKCNT; i++) {
				if (ProcessTable[i].remaining_ticks > remaining_tick_max) {
					remaining_tick_max = ProcessTable[i].remaining_ticks;
					ProcessReady = &ProcessTable[i];
				}
			}
			if (remaining_tick_max == 0) {
				for (DWORD i = 0; i < KRNL_PROC_MAXTASKCNT; i++) {
					ProcessTable[i].remaining_ticks = ProcessTable[i].priority;
				}
			}
		}
	}
}
//----------------------------
//    键盘I/O
//----------------------------
DWORD KC_KB_GetScanCode() {
	return AF_InPort(0x60);
}