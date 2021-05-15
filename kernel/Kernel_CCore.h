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
#include "Kernel_VESACharMap.h"

VOID KC_KB_KeyboardHandler(DWORD);
VOID KC_CON_OutputChar(CONSOLE*, CHAR);
VOID KC_TTY_SysCallWrite(TTY*, CHAR*, DWORD);

DWORD CSTD_printf(CONST CHAR* format_string, ...);

//----------------------------
//    断言
//----------------------------

#define KC_Assert(exp) if(!(exp))KC_AssertFail(#exp,__FILE__,__BASE_FILE__,__LINE__);
#define KC_Proc2Pid(x) (x - ProcessTable)
#define KC_PrintL CSTD_printf

DWORD KC_LDT_SEG_Linear( PROCESS* p, DWORD idx)
{
	DESCRIPTOR* d = &p->ldt[idx];

	return d->base_high << 24 | d->base_mid << 16 | d->base_low;
}
VOID* KC_VA2LA(DWORD pid, VOID* va)
{
	PROCESS* p = &ProcessTable[pid];
	DWORD seg_base = KC_LDT_SEG_Linear(p, 1);
	DWORD la = seg_base + (DWORD)va;
	if (pid < KRNL_PROC_MAXCNT) {
		KC_Assert(la == (DWORD)va);
	}
	return (VOID*)la;
}

VOID KC_AssertFail(CHAR * exp, CHAR * file, CHAR * basefile, DWORD line) {
	KC_PrintL("%c  Assert(%s) Failed:\n File:%s Basefile:%s",
		KRNL_MAG_CH_ASSERT, exp, file, basefile, line);
	while (1);
}

DWORD KCHD_SysCall_Printx(DWORD _unu1, DWORD _unu2, CHAR* s, PROCESS* proc) {
	CONST CHAR* p;
	CHAR ch;
	CHAR reenter_error[] = "? K_Reenter Is Incorrect!";
	reenter_error[0] = KRNL_MAG_CH_PANIC;

	
	if (K_IntReenter == 0) {
		p = KC_VA2LA(KC_Proc2Pid(proc), s);
	}
	else if (K_IntReenter > 0) {
		p = s;
	}
	else {
		p = reenter_error;
	}

	if ((*p == KRNL_MAG_CH_PANIC) || (*p == KRNL_MAG_CH_ASSERT && ProcessReady->privilege == KRNL_PROC_RINGPRIV_TSK)) {
	//if ((*p == KRNL_MAG_CH_PANIC) || (*p == KRNL_MAG_CH_ASSERT)) {
		GCCASM_INTEL_SYNTAX;
		asm("cli");
		CONST CHAR* q = p + 1;
		DWORD r = 0;
		DWORD c = 0;
		while (*q) {
			if ((*q) == '\n') {
				q++;
				r++;
				c = 0;
			}
			else {
				AF_VMBreakPoint();
				KC_VESA_PutChar(*q++, r, c++, 255, 255, 255);
		
			}
			
			
		}
		asm("hlt");
	}
	
	while ((ch = *p++) != 0) {
		if (ch == KRNL_MAG_CH_PANIC || ch == KRNL_MAG_CH_ASSERT) {
			continue;
		}
		KC_CON_OutputChar(KRNL_TTY_Table[proc->tty_id].bound_con, ch);
	}
}
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
		AF_OutPort(KRNL_INT_M_CTLMASK, AF_InPort(KRNL_INT_M_CTLMASK) & ~(1 << irq_id));
	}
	else {
		AF_OutPort(KRNL_INT_S_CTLMASK, AF_InPort(KRNL_INT_S_CTLMASK) & ~(1 << irq_id));
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

VOID KC_LoadProcessInfo(PROCESS* proc, SELECTOR_W ldt_selector, SELECTOR_S cs, SELECTOR_S ds, SELECTOR_S es, SELECTOR_S fs,
	SELECTOR_S gs, SELECTOR_S ss, HANDLER proc_entry, DWORD stack_address, DWORD eflag) {
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
}
VOID KC_DuplicateDescriptor(DESCRIPTOR* dest, DESCRIPTOR* src) {
	AF_MemoryCopy(dest, src, sizeof(DESCRIPTOR));
}
VOID KC_DuplicateGlobalDescriptor(DESCRIPTOR* dest, SELECTOR_W gdt_selector) {
	KC_DuplicateDescriptor(dest, &GDT[gdt_selector >> 3]);
}
VOID KC_DuplicateGlobalDescriptorEx(DESCRIPTOR* dest, SELECTOR_W gdt_selector, UDWORD attr1) {
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

DWORD KCHD_SysCall_ConWrite(CHAR* buf,DWORD len, PROCESS* proc) {
	if (proc->tty_id == KRNL_PROC_TTY_NULL) {
		return;
	}
	KC_TTY_SysCallWrite(&KRNL_TTY_Table[proc->tty_id], buf, len);
}

VOID KC_SysCall_Establish(UDWORD id, VOID* handler) {
	syscall_table[id] = handler;
}

VOID KCHD_SysCall_SendRec(DWORD function, DWORD src_dst, MESSAGE* m, PROCESS* proc) {
	KC_Assert(K_IntReenter == 0);
	KC_Assert((src_dst >= 0 && src_dst < KRNL_PROC_MAXTASKCNT) || src_dst == KRNL_SNDREC_ANY || src_dst == KRNL_SNDREC_INTERRUPT);
	DWORD ret = 0;
	DWORD caller = KC_Proc2Pid(proc);
	MESSAGE* mla = (MESSAGE*)KC_VA2LA(caller, m);
	mla->source = caller;

	KC_Assert(mla->source != src_dst);
	if (function == KRNL_SNDREC_SEND) {
		//ret = KC_MessageSend(proc, src_dst, m);
		if (ret != 0) {
			return ret;
		}
	}
	else if (function == KRNL_SNDREC_RECEIVE) {
		//ret = KC_MessageReceive(proc, src_dst, m);
		if (ret != 0) {
			return ret;
		}
	}
	else {
		//KC_Panic("{SendRec} Invalid Function: %d (Send:%d, Receive%d)", function, KRNL_SNDREC_SEND, KRNL_SNDREC_RECEIVE);
	}

}
//VOID KCHD_SysCall_SendRec(DWORD func,DWORD src_dst)
//----------------------------
//    进程和作业
//----------------------------
VOID KC_LoadTaskTable(UWORD id, HANDLER handler, DWORD stacksize, CHAR* taskname, UBYTE* stack_addr, DWORD priority, DWORD privilege) {
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
	task_table[id].privilege = privilege;
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
//    显示
//----------------------------

VOID KC_VESA_PutPixel(DWORD row, DWORD col, UBYTE r, UBYTE g, UBYTE b) {
	DWORD realx = row * 1, realy = col * 1;
	DWORD pxPos = realx * VESA_RES_W + realy;
	DWORD pxIdx = pxPos * 3;
	KRNL_VESA_FrameBuffer[pxPos].R = r;
	KRNL_VESA_FrameBuffer[pxPos].G = g;
	KRNL_VESA_FrameBuffer[pxPos].B = b;
	AF_VESA_PutPixel(pxIdx, b, g, r);
}

VOID KC_VESA_SwitchBuffer(VESA_FRAMEBUFFER fb) {
	GCCASM_INTEL_SYNTAX;
	asm("cli");
	AF_VESA_ScreenMemPaste(0, fb, sizeof(KRNL_VESA_FrameBuffer));
	AF_MemoryCopy(KRNL_VESA_FrameBuffer, fb, sizeof(KRNL_VESA_FrameBuffer));
	KRNL_DISP_ActivatedBuffer = fb;
	asm("sti");
}

VOID KC_VESA_PutChar(CHAR ch, DWORD row, DWORD col, UBYTE r, UBYTE g, UBYTE b) {
	DWORD basex = row * VESA_FONT_ROWS, basey = col * VESA_FONT_COLS;
	for (DWORD i = basex; i < basex + VESA_FONT_ROWS; i++) {
		for (DWORD j = basey; j < basey + VESA_FONT_COLS; j++) {
			if (KRNL_VIDEO_CHARMAP[ch][(i - basex) * VESA_FONT_COLS + (j - basey)] == 1) {
				KC_VESA_PutPixel(i, j, r, g, b);
			}
			else {
				KC_VESA_PutPixel(i, j, 0, 0, 0);
			}
		}
	}
}
VOID KC_VESA_PutPixelBuf(VESA_PIXEL* buf, DWORD row, DWORD col, UBYTE r, UBYTE g, UBYTE b) {
	DWORD realx = row * 1, realy = col * 1;
	DWORD pxPos = realx * VESA_RES_W + realy;
	DWORD pxIdx = pxPos * 3;
	buf[pxPos].R = r;
	buf[pxPos].G = g;
	buf[pxPos].B = b;
	if (buf == KRNL_DISP_ActivatedBuffer) {
		KC_VESA_PutPixel(row, col, r, g, b);
	}
}

VOID KC_VESA_PutCharBuf(VESA_PIXEL* buf,CHAR ch, DWORD row, DWORD col, UBYTE r, UBYTE g, UBYTE b) {
	DWORD basex = row * VESA_FONT_ROWS, basey = col * VESA_FONT_COLS;
	for (int i = basex; i < basex + VESA_FONT_ROWS; i++) {
		for (int j = basey; j < basey + VESA_FONT_COLS; j++) {
			if (KRNL_VIDEO_CHARMAP[ch][(i - basex) * VESA_FONT_COLS + (j - basey)] == 1) {
				KC_VESA_PutPixelBuf(buf,i, j, r, g, b);
			}
			else {
				KC_VESA_PutPixelBuf(buf,i, j, 0, 0, 0);
			}
		}
	}
}

VOID KC_VESA_ScrollUp(DWORD lines) {
	UBYTE* beginAddr = &KRNL_VESA_FrameBuffer[lines * VESA_RES_W];
	UBYTE* bgAddr = KRNL_VESA_FrameBuffer;
	GCCASM_INTEL_SYNTAX;
	asm("cli");
	AF_VESA_ScreenMemPasteDWord(0, beginAddr, sizeof(KRNL_VESA_FrameBuffer) - lines * VESA_RES_W * sizeof(VESA_PIXEL));
	AF_VESA_ScreenMemClear(sizeof(KRNL_VESA_FrameBuffer) - lines * VESA_RES_W * sizeof(VESA_PIXEL), lines * VESA_RES_W * sizeof(VESA_PIXEL));
	AF_MemoryCopy(bgAddr, beginAddr, sizeof(KRNL_VESA_FrameBuffer) - lines * VESA_RES_W * sizeof(VESA_PIXEL));
	AF_VESA_ScreenMemClear(bgAddr + sizeof(KRNL_VESA_FrameBuffer) - lines * VESA_RES_W * sizeof(VESA_PIXEL), lines * VESA_RES_W * sizeof(VESA_PIXEL));
	asm("sti");
}

VOID KC_VESA_FontScrollUp(DWORD char_lines) {
	KC_VESA_ScrollUp(char_lines * VESA_FONT_ROWS);
	KRNL_VESAFont_Row--;
}

VOID KC_VESA_ClearBuffer(VESA_PIXEL* pxbuffer, DWORD size) {
	for (int i = 0; i < size; i++) {
		pxbuffer[i].R = 0;
		pxbuffer[i].G = 0;
		pxbuffer[i].B = 0;
	}
}

//----------------------------
//    TTY
//----------------------------
VOID KC_TTY_Init(TTY* tp) {
	tp->input_buffer_cursize = 0;
	tp->input_buffer.front = tp->input_buffer.end = 0;
	DWORD w = tp - KRNL_TTY_Table;
	tp->bound_con = w + KRNL_CON_Table;
	//tp->bound_con->disp_buf = (KRNL_CON_VFrameBuffer + sizeof(KRNL_VESA_FrameBuffer) * w);
}
VOID KC_TTY_InitCon() {
	KRNL_TTY_Table[0].bound_con->disp_buf = KRNL_CON_VFrameBuffer_0;
	KRNL_TTY_Table[1].bound_con->disp_buf = KRNL_CON_VFrameBuffer_1;
}

VOID KC_TTY_SysCallWrite(TTY* tty, CHAR* buf, DWORD len) {
	CHAR* p = buf;
	DWORD i = len;
	while (i--) {
		KC_CON_OutputChar(tty->bound_con, *p++);
	}
}

VOID KC_CON_SelectConsole(DWORD idx) {
	KRNL_CON_CurConsole = idx;
	KC_VESA_SwitchBuffer(KRNL_CON_Table[idx].disp_buf);
}

VOID KC_TTY_PutKey(TTY* tp, UDWORD key) {
	if ((tp->input_buffer.end + 1) % KRNL_TTY_BUF_SIZE != (tp->input_buffer.front)) {
		tp->input_buffer.buffer[tp->input_buffer.end] = key;
		tp->input_buffer.end++;
		tp->input_buffer.end %= KRNL_TTY_BUF_SIZE;
	}
}

VOID KC_TTY_InProc(TTY* tp, UDWORD key) {
	if (!(key & KRNL_KB_FLAG_EXT)) {
		if ((tp->input_buffer.end + 1) % KRNL_TTY_BUF_SIZE != (tp->input_buffer.front)) {
			tp->input_buffer.buffer[tp->input_buffer.end] = key;
			tp->input_buffer.end++;
			tp->input_buffer.end %= KRNL_TTY_BUF_SIZE;
		}
	}
	else {
		DWORD raw_code = key & KRNL_KB_MASK_RAW;
		switch (raw_code) {
			case KRNL_KB_F1:
			case KRNL_KB_F2:
				KC_CON_SelectConsole(raw_code - KRNL_KB_F1);
				break;
			case KRNL_KB_F3:
				
				break;

			case KRNL_KB_ENTER:
				KC_TTY_PutKey(tp, '\n');
				break;
			case KRNL_KB_BACKSPACE:
				KC_TTY_PutKey(tp, '\b');
				break;
			default:
				break;
		}
	}
	
}

VOID KC_TTY_Read(TTY* tp) {
	
	if (KC_CON_IsActiveConsole(tp->bound_con)) {
		
		KC_KB_ScancodeRead(tp);
	}
}


VOID KC_CON_OutputChar(CONSOLE* p, CHAR ch) {
	if (ch != '\n') {
		switch (ch) {
			case '\b':
				if (p->ch_col > 0) {
					p->ch_col--;
				}
				KC_VESA_PutCharBuf(p->disp_buf, ' ', p->ch_row, p->ch_col, 0xff, 0xff, 0xff);

				break;
			default:
				KC_VESA_PutCharBuf(p->disp_buf, ch, p->ch_row, p->ch_col, 0xff, 0xff, 0xff);
				p->ch_col++;
		}
		
	}
	if (p->ch_col == VESA_FONT_COLMAX||ch=='\n') {
		p->ch_row++;
		p->ch_col = 0;
	}
}


VOID KC_CON_OutputStr(CONSOLE* p, CHAR* ch) {
	CHAR* st = ch;
	while (*st != '\0') {
		KC_CON_OutputChar(p, *st);
		st++;
	}
}

VOID KC_TTY_Write(TTY* tp) {
	
	if (tp->input_buffer.end != tp->input_buffer.front) {
		CHAR ch = tp->input_buffer.buffer[tp->input_buffer.front++];
		tp->input_buffer.front %= KRNL_TTY_BUF_SIZE;
		
		KC_CON_OutputChar(tp->bound_con, ch);
		
	}
	
}

VOID KC_TTY_CyclicExecution() {
	TTY* tp;
	for (tp = KRNL_TTY_Table; tp < KRNL_TTY_Table + KRNL_CON_COUNT; tp++) {
		KC_TTY_Init(tp);
	}
	KC_TTY_InitCon();
	KRNL_CON_CurConsole = 0;
	KC_CON_SelectConsole(0);

	while (1) {
		for (tp = KRNL_TTY_Table; tp < KRNL_TTY_Table + KRNL_CON_COUNT; tp++) {
			KC_TTY_Read(tp);
			KC_TTY_Write(tp);
			KC_Assert(0);
		}
		
	}
}

DWORD KC_CON_IsActiveConsole(CONSOLE* con) {
	
	if (con == &KRNL_CON_Table[KRNL_CON_CurConsole]) {
		
		return TRUE;
	}
	return FALSE;
}


//----------------------------
//    键盘I/O
//----------------------------
DWORD KC_KB_GetScanCode() {
	return AF_InPort(0x60);
}

//等待8042缓冲区空
VOID KC_KB_Wait() {
	UBYTE kbst;
	do {
		kbst = AF_InPort(0x64);

	} while (kbst & 0x02);
}

VOID KC_KB_ReturnAck() {
	UBYTE kbst;
	do {
		kbst = AF_InPort(0x60);

	} while (kbst!=0xFA);
}

VOID KC_KB_SetLED() {
	UBYTE led = (KRNL_KB_NumLock << 1) | (KRNL_KB_CapLock << 2) | KRNL_KB_ScrollLock;
	KC_KB_Wait();
	AF_OutPort(0x60, 0xED);
	KC_KB_ReturnAck();
	KC_KB_Wait();
	AF_OutPort(0x60, led);
	KC_KB_ReturnAck();
}

VOID KC_KB_KeyboardHandler(DWORD x) {
	DWORD kp = KC_KB_GetScanCode();
	if (kb_buffer.front != (kb_buffer.end + 1) % KB_BUFFER_CAPACITY) {
		kb_buffer.buffer[kb_buffer.end] = (BYTE)(kp);
		kb_buffer.end++;
		kb_buffer.end %= KB_BUFFER_CAPACITY;
	}
}

DWORD KC_KB_KeyBoardRead() {
	if (kb_buffer.front != kb_buffer.end) {
		DWORD rtn = kb_buffer.buffer[kb_buffer.front];
		kb_buffer.front++;
		kb_buffer.front %= KB_BUFFER_CAPACITY;
		return rtn;
	}
	return -1;
}

VOID KC_KB_ScancodeRead(TTY* tp) {
	UBYTE scancode;
	UBYTE output[2] = { 0,0 };
	BOOL isMake;
	GCCASM_INTEL_SYNTAX;
	asm("cli");
	UBYTE kbread = KC_KB_KeyBoardRead();
	UWORD* krow;
	UDWORD kcol;
	UDWORD key;
	asm("sti");
	if (kbread != -1) {

		scancode = kbread;
		if (scancode == 0xe1) {

		}
		else if (scancode == 0xe0) {
			KRNL_KB_E0_Flag = TRUE;
		}
		else {

			isMake = ((scancode & 0x80) == 0);
			krow = &KRNL_KeyMap[(scancode & 0x7F) * 3];
			kcol = 0;
			if (KRNL_KB_ShiftL || KRNL_KB_ShiftR) {
				kcol = 1;
			}
			if (KRNL_KB_E0_Flag) {
				kcol = 2;
				KRNL_KB_E0_Flag = FALSE;
			}
			

			int shift_cap = KRNL_KB_ShiftL | KRNL_KB_ShiftR;
			if (KRNL_KB_CapLock) {
				if (krow[0] >= 'a' && krow[0] <= 'z') {
					kcol = !kcol;
				}
			}
			key = krow[kcol];

			switch (key)
			{
				case KRNL_KB_SHIFT_L:
					KRNL_KB_ShiftL = isMake;
					//key = 0;
					break;
				case KRNL_KB_SHIFT_R:
					KRNL_KB_ShiftR = isMake;
					//key = 0;
					break;
				case KRNL_KB_CTRL_L:
					KRNL_KB_CtrlL = isMake;
					//key = 0;
					break;
				case KRNL_KB_CTRL_R:
					KRNL_KB_CtrlR = isMake;
					//key = 0;
					break;
				case KRNL_KB_ALT_L:
					KRNL_KB_AltL = isMake;
					//key = 0;
					break;
				case KRNL_KB_ALT_R:
					KRNL_KB_AltR = isMake;
					//key = 0;
					break;
				case KRNL_KB_CAPS_LOCK:
					if (isMake) {
						KRNL_KB_CapLock = !KRNL_KB_CapLock;
						KC_KB_SetLED();
					}
					break;
				case KRNL_KB_NUM_LOCK:
					if (isMake) {
						KRNL_KB_NumLock = !KRNL_KB_NumLock;
						KC_KB_SetLED();
					}
					break;
				case KRNL_KB_SCROLL_LOCK:
					if (isMake) {
						KRNL_KB_ScrollLock = !KRNL_KB_ScrollLock;
						KC_KB_SetLED();
					}
					break;
				default:
					if (!isMake) {
						key = 0;
					}
					break;
			}
			if (isMake) {
				int spad = 0;
				if ((key >= KRNL_KB_PAD_SLASH) && key <= KRNL_KB_PAD_9) {
					spad = 1;
					switch (key) {
						case KRNL_KB_PAD_SLASH:
							key = '/';
							break;
						case KRNL_KB_PAD_STAR:
							key = '*';
							break;
						case KRNL_KB_PAD_MINUS:
							key = '-';
							break;
						case KRNL_KB_PAD_PLUS:
							key = '+';
							break;
						case KRNL_KB_PAD_ENTER:
							key = KRNL_KB_ENTER;
							break;
						default:
							if (KRNL_KB_NumLock && key >= KRNL_KB_PAD_0 && key <= KRNL_KB_PAD_9) {
								key = key - KRNL_KB_PAD_0 + '0';
							}
							else if (KRNL_KB_NumLock && key == KRNL_KB_PAD_DOT) {
								key = '.';

							}
							else {
								switch (key) {
									case KRNL_KB_PAD_HOME:
										key = KRNL_KB_HOME;
										break;
									case KRNL_KB_PAD_END:
										key = KRNL_KB_END;
										break;
									case KRNL_KB_PAD_PAGEUP:
										key = KRNL_KB_PAGEUP;
										break;
									case KRNL_KB_PAD_PAGEDOWN:
										key = KRNL_KB_PAD_PAGEDOWN;
										break;
									case KRNL_KB_PAD_INS:
										key = KRNL_KB_INSERT;
										break;
									case KRNL_KB_PAD_UP:
										key = KRNL_KB_UP;
										break;
									case KRNL_KB_PAD_DOWN:
										key = KRNL_KB_DOWN;
										break;
									case KRNL_KB_PAD_LEFT:
										key = KRNL_KB_LEFT;
										break;
									case KRNL_KB_PAD_RIGHT:
										key = KRNL_KB_RIGHT;
										break;
									case KRNL_KB_PAD_DOT:
										key = KRNL_KB_DELETE;
										break;
									default :
										break;
								}
							}
					}
				}
				key |= KRNL_KB_ShiftL ? KRNL_KB_FLAG_SHIFT_L : 0;
				key |= KRNL_KB_ShiftR ? KRNL_KB_FLAG_SHIFT_R : 0;
				key |= KRNL_KB_CtrlL ? KRNL_KB_FLAG_CTRL_L : 0;
				key |= KRNL_KB_CtrlR ? KRNL_KB_FLAG_CTRL_R : 0;
				key |= KRNL_KB_AltL ? KRNL_KB_FLAG_ALT_L : 0;
				key |= KRNL_KB_AltR ? KRNL_KB_FLAG_ALT_R : 0;
				key |= spad ? KRNL_KB_FLAG_PAD : 0;

				KC_TTY_InProc(tp, key);
			}
		}
	}
}


//扫描码映射
//更改函数内容请同时更改Kernel_CDef.h中的宏定义
VOID KC_KB_InitScanCodeMapping() {
	for (int i = 0; i < 384; i++) {
		KRNL_KeyMap[i] = 0;
	}
	KRNL_KeyMap[1] = 0;
	KRNL_KeyMap[2] = 0;
	KRNL_KeyMap[3] = 257;
	KRNL_KeyMap[4] = 257;
	KRNL_KeyMap[5] = 0;
	KRNL_KeyMap[6] = 49;
	KRNL_KeyMap[7] = 33;
	KRNL_KeyMap[8] = 0;
	KRNL_KeyMap[9] = 50;
	KRNL_KeyMap[10] = 64;
	KRNL_KeyMap[11] = 0;
	KRNL_KeyMap[12] = 51;
	KRNL_KeyMap[13] = 35;
	KRNL_KeyMap[14] = 0;
	KRNL_KeyMap[15] = 52;
	KRNL_KeyMap[16] = 36;
	KRNL_KeyMap[17] = 0;
	KRNL_KeyMap[18] = 53;
	KRNL_KeyMap[19] = 37;
	KRNL_KeyMap[20] = 0;
	KRNL_KeyMap[21] = 54;
	KRNL_KeyMap[22] = 94;
	KRNL_KeyMap[23] = 0;
	KRNL_KeyMap[24] = 55;
	KRNL_KeyMap[25] = 38;
	KRNL_KeyMap[26] = 0;
	KRNL_KeyMap[27] = 56;
	KRNL_KeyMap[28] = 42;
	KRNL_KeyMap[29] = 0;
	KRNL_KeyMap[30] = 57;
	KRNL_KeyMap[31] = 40;
	KRNL_KeyMap[32] = 0;
	KRNL_KeyMap[33] = 48;
	KRNL_KeyMap[34] = 41;
	KRNL_KeyMap[35] = 0;
	KRNL_KeyMap[36] = 45;
	KRNL_KeyMap[37] = 95;
	KRNL_KeyMap[38] = 0;
	KRNL_KeyMap[39] = 61;
	KRNL_KeyMap[40] = 43;
	KRNL_KeyMap[41] = 0;
	KRNL_KeyMap[42] = 260;
	KRNL_KeyMap[43] = 260;
	KRNL_KeyMap[44] = 0;
	KRNL_KeyMap[45] = 258;
	KRNL_KeyMap[46] = 258;
	KRNL_KeyMap[47] = 0;
	KRNL_KeyMap[48] = 113;
	KRNL_KeyMap[49] = 81;
	KRNL_KeyMap[50] = 0;
	KRNL_KeyMap[51] = 119;
	KRNL_KeyMap[52] = 87;
	KRNL_KeyMap[53] = 0;
	KRNL_KeyMap[54] = 101;
	KRNL_KeyMap[55] = 69;
	KRNL_KeyMap[56] = 0;
	KRNL_KeyMap[57] = 114;
	KRNL_KeyMap[58] = 82;
	KRNL_KeyMap[59] = 0;
	KRNL_KeyMap[60] = 116;
	KRNL_KeyMap[61] = 84;
	KRNL_KeyMap[62] = 0;
	KRNL_KeyMap[63] = 121;
	KRNL_KeyMap[64] = 89;
	KRNL_KeyMap[65] = 0;
	KRNL_KeyMap[66] = 117;
	KRNL_KeyMap[67] = 85;
	KRNL_KeyMap[68] = 0;
	KRNL_KeyMap[69] = 105;
	KRNL_KeyMap[70] = 73;
	KRNL_KeyMap[71] = 0;
	KRNL_KeyMap[72] = 111;
	KRNL_KeyMap[73] = 79;
	KRNL_KeyMap[74] = 0;
	KRNL_KeyMap[75] = 112;
	KRNL_KeyMap[76] = 80;
	KRNL_KeyMap[77] = 0;
	KRNL_KeyMap[78] = 91;
	KRNL_KeyMap[79] = 123;
	KRNL_KeyMap[80] = 0;
	KRNL_KeyMap[81] = 93;
	KRNL_KeyMap[82] = 125;
	KRNL_KeyMap[83] = 0;
	KRNL_KeyMap[84] = 259;
	KRNL_KeyMap[85] = 259;
	KRNL_KeyMap[86] = 304;
	KRNL_KeyMap[87] = 266;
	KRNL_KeyMap[88] = 266;
	KRNL_KeyMap[89] = 267;
	KRNL_KeyMap[90] = 97;
	KRNL_KeyMap[91] = 65;
	KRNL_KeyMap[92] = 0;
	KRNL_KeyMap[93] = 115;
	KRNL_KeyMap[94] = 83;
	KRNL_KeyMap[95] = 0;
	KRNL_KeyMap[96] = 100;
	KRNL_KeyMap[97] = 68;
	KRNL_KeyMap[98] = 0;
	KRNL_KeyMap[99] = 102;
	KRNL_KeyMap[100] = 70;
	KRNL_KeyMap[101] = 0;
	KRNL_KeyMap[102] = 103;
	KRNL_KeyMap[103] = 71;
	KRNL_KeyMap[104] = 0;
	KRNL_KeyMap[105] = 104;
	KRNL_KeyMap[106] = 72;
	KRNL_KeyMap[107] = 0;
	KRNL_KeyMap[108] = 106;
	KRNL_KeyMap[109] = 74;
	KRNL_KeyMap[110] = 0;
	KRNL_KeyMap[111] = 107;
	KRNL_KeyMap[112] = 75;
	KRNL_KeyMap[113] = 0;
	KRNL_KeyMap[114] = 108;
	KRNL_KeyMap[115] = 76;
	KRNL_KeyMap[116] = 0;
	KRNL_KeyMap[117] = 59;
	KRNL_KeyMap[118] = 58;
	KRNL_KeyMap[119] = 0;
	KRNL_KeyMap[120] = 39;
	KRNL_KeyMap[121] = 34;
	KRNL_KeyMap[122] = 0;
	KRNL_KeyMap[123] = 96;
	KRNL_KeyMap[124] = 126;
	KRNL_KeyMap[125] = 0;
	KRNL_KeyMap[126] = 264;
	KRNL_KeyMap[127] = 264;
	KRNL_KeyMap[128] = 0;
	KRNL_KeyMap[129] = 92;
	KRNL_KeyMap[130] = 124;
	KRNL_KeyMap[131] = 0;
	KRNL_KeyMap[132] = 122;
	KRNL_KeyMap[133] = 90;
	KRNL_KeyMap[134] = 0;
	KRNL_KeyMap[135] = 120;
	KRNL_KeyMap[136] = 88;
	KRNL_KeyMap[137] = 0;
	KRNL_KeyMap[138] = 99;
	KRNL_KeyMap[139] = 67;
	KRNL_KeyMap[140] = 0;
	KRNL_KeyMap[141] = 118;
	KRNL_KeyMap[142] = 86;
	KRNL_KeyMap[143] = 0;
	KRNL_KeyMap[144] = 98;
	KRNL_KeyMap[145] = 66;
	KRNL_KeyMap[146] = 0;
	KRNL_KeyMap[147] = 110;
	KRNL_KeyMap[148] = 78;
	KRNL_KeyMap[149] = 0;
	KRNL_KeyMap[150] = 109;
	KRNL_KeyMap[151] = 77;
	KRNL_KeyMap[152] = 0;
	KRNL_KeyMap[153] = 44;
	KRNL_KeyMap[154] = 60;
	KRNL_KeyMap[155] = 0;
	KRNL_KeyMap[156] = 46;
	KRNL_KeyMap[157] = 62;
	KRNL_KeyMap[158] = 0;
	KRNL_KeyMap[159] = 47;
	KRNL_KeyMap[160] = 63;
	KRNL_KeyMap[161] = 300;
	KRNL_KeyMap[162] = 265;
	KRNL_KeyMap[163] = 265;
	KRNL_KeyMap[164] = 0;
	KRNL_KeyMap[165] = 42;
	KRNL_KeyMap[166] = 42;
	KRNL_KeyMap[167] = 0;
	KRNL_KeyMap[168] = 268;
	KRNL_KeyMap[169] = 268;
	KRNL_KeyMap[170] = 269;
	KRNL_KeyMap[171] = 32;
	KRNL_KeyMap[172] = 32;
	KRNL_KeyMap[173] = 0;
	KRNL_KeyMap[174] = 270;
	KRNL_KeyMap[175] = 270;
	KRNL_KeyMap[176] = 0;
	KRNL_KeyMap[177] = 273;
	KRNL_KeyMap[178] = 273;
	KRNL_KeyMap[179] = 0;
	KRNL_KeyMap[180] = 274;
	KRNL_KeyMap[181] = 274;
	KRNL_KeyMap[182] = 0;
	KRNL_KeyMap[183] = 275;
	KRNL_KeyMap[184] = 275;
	KRNL_KeyMap[185] = 0;
	KRNL_KeyMap[186] = 276;
	KRNL_KeyMap[187] = 276;
	KRNL_KeyMap[188] = 0;
	KRNL_KeyMap[189] = 277;
	KRNL_KeyMap[190] = 277;
	KRNL_KeyMap[191] = 0;
	KRNL_KeyMap[192] = 278;
	KRNL_KeyMap[193] = 278;
	KRNL_KeyMap[194] = 0;
	KRNL_KeyMap[195] = 279;
	KRNL_KeyMap[196] = 279;
	KRNL_KeyMap[197] = 0;
	KRNL_KeyMap[198] = 280;
	KRNL_KeyMap[199] = 280;
	KRNL_KeyMap[200] = 0;
	KRNL_KeyMap[201] = 281;
	KRNL_KeyMap[202] = 281;
	KRNL_KeyMap[203] = 0;
	KRNL_KeyMap[204] = 282;
	KRNL_KeyMap[205] = 282;
	KRNL_KeyMap[206] = 0;
	KRNL_KeyMap[207] = 271;
	KRNL_KeyMap[208] = 271;
	KRNL_KeyMap[209] = 0;
	KRNL_KeyMap[210] = 272;
	KRNL_KeyMap[211] = 272;
	KRNL_KeyMap[212] = 0;
	KRNL_KeyMap[213] = 313;
	KRNL_KeyMap[214] = 55;
	KRNL_KeyMap[215] = 289;
	KRNL_KeyMap[216] = 314;
	KRNL_KeyMap[217] = 56;
	KRNL_KeyMap[218] = 293;
	KRNL_KeyMap[219] = 315;
	KRNL_KeyMap[220] = 57;
	KRNL_KeyMap[221] = 291;
	KRNL_KeyMap[222] = 302;
	KRNL_KeyMap[223] = 45;
	KRNL_KeyMap[224] = 0;
	KRNL_KeyMap[225] = 310;
	KRNL_KeyMap[226] = 52;
	KRNL_KeyMap[227] = 295;
	KRNL_KeyMap[228] = 311;
	KRNL_KeyMap[229] = 53;
	KRNL_KeyMap[230] = 0;
	KRNL_KeyMap[231] = 312;
	KRNL_KeyMap[232] = 54;
	KRNL_KeyMap[233] = 296;
	KRNL_KeyMap[234] = 303;
	KRNL_KeyMap[235] = 43;
	KRNL_KeyMap[236] = 0;
	KRNL_KeyMap[237] = 307;
	KRNL_KeyMap[238] = 49;
	KRNL_KeyMap[239] = 290;
	KRNL_KeyMap[240] = 308;
	KRNL_KeyMap[241] = 50;
	KRNL_KeyMap[242] = 294;
	KRNL_KeyMap[243] = 309;
	KRNL_KeyMap[244] = 51;
	KRNL_KeyMap[245] = 292;
	KRNL_KeyMap[246] = 306;
	KRNL_KeyMap[247] = 48;
	KRNL_KeyMap[248] = 287;
	KRNL_KeyMap[249] = 305;
	KRNL_KeyMap[250] = 46;
	KRNL_KeyMap[251] = 288;
	KRNL_KeyMap[252] = 0;
	KRNL_KeyMap[253] = 0;
	KRNL_KeyMap[254] = 0;
	KRNL_KeyMap[255] = 0;
	KRNL_KeyMap[256] = 0;
	KRNL_KeyMap[257] = 0;
	KRNL_KeyMap[258] = 0;
	KRNL_KeyMap[259] = 0;
	KRNL_KeyMap[260] = 0;
	KRNL_KeyMap[261] = 283;
	KRNL_KeyMap[262] = 283;
	KRNL_KeyMap[263] = 0;
	KRNL_KeyMap[264] = 284;
	KRNL_KeyMap[265] = 284;
	KRNL_KeyMap[266] = 0;
	KRNL_KeyMap[267] = 0;
	KRNL_KeyMap[268] = 0;
	KRNL_KeyMap[269] = 0;
	KRNL_KeyMap[270] = 0;
	KRNL_KeyMap[271] = 0;
	KRNL_KeyMap[272] = 0;
	KRNL_KeyMap[273] = 0;
	KRNL_KeyMap[274] = 0;
	KRNL_KeyMap[275] = 261;
	KRNL_KeyMap[276] = 0;
	KRNL_KeyMap[277] = 0;
	KRNL_KeyMap[278] = 262;
	KRNL_KeyMap[279] = 0;
	KRNL_KeyMap[280] = 0;
	KRNL_KeyMap[281] = 263;

}

//----------------------------
//    控制台输出
//----------------------------

DWORD CSTD_vsprintf(CHAR* buffer, CONST CHAR* format_string, VA_LIST arg) {
	CHAR* p;
	CHAR tmp[KRNL_CON_OUTPUT_BUFSIZE];
	VA_LIST carg = arg;
	for (p = buffer; *format_string; format_string++) {
		if (*format_string != '%') {
			*p++ = *format_string;
			//format_string++;
			continue;
		}
		format_string++;
		CHAR tmp;
		CHAR* tmp2;
		switch (*format_string) {
			case 'c':
				tmp = VA_ARG(carg, CHAR);
				*p++ = tmp;
				break;
			case 's':
				tmp2 = VA_ARG(carg, CHAR*);
				while (*tmp2) {
					*p++ = *tmp2++;
				}
				
				break;
			default:
				continue;
		}

	}
	*p = 0;
	return (p - buffer);
}

DWORD CSTD_printf(CONST CHAR* format_string,...) {
	CHAR buffer[KRNL_CON_OUTPUT_BUFSIZE];
	VA_LIST arg;
	VA_START(arg, format_string);
	DWORD i = CSTD_vsprintf(buffer, format_string, arg);

	//KCEX_PrintFormat("(%d)", i);
	buffer[i] = 0;
	SYSCALL_ConWriteX(buffer);
	return i;
}