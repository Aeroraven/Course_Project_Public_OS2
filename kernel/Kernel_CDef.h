#pragma once
// ---------------------------------------------------------------			
// Kernel_CDef.h
// ---------------------------------------------------------------	
// Operating System Kernel Definition Header
// OS内核 - C-定义
// By Aeroraven, 2021-4-3
// --------------------------------------------------------------

//变量声明
#define EXTERN extern

//调用定义
#define CDECL _cdecl
#define STDCALL _stdcall

//类型定义
typedef unsigned long long UQWORD;
typedef unsigned int UDWORD; 		//双字，等效于unsigned int
typedef unsigned short UWORD;	//字，等效于unsigned short
typedef unsigned char UBYTE;		//字节，等效于unsigned char;

typedef long long QWORD;
typedef int DWORD;		//双字，等效于int
typedef short WORD;		//字，等效于short
typedef char  BYTE;		//字节，等效于char

typedef void* ANYPTR;	//空类型指针,等效于UDWORD
typedef void* GDTPTR;	//GDT指针
typedef void* LDTPTR;	//LDT指针
typedef void* IDTPTR;   //IDT指针
typedef UDWORD SELECTOR_W;

typedef void VOID;
typedef char CHAR;
typedef char BOOL;

typedef char* CCSTRING; //字符串指针
typedef void (*HANDLER)(); //函数指针
typedef UDWORD (*UDWORD_HANDLER)(); //函数指针

typedef char* VA_LIST; //可变参数列表(不安全)

typedef float FLOAT;
typedef double DOUBLE;

#define _INTSIZEOF(n) ((sizeof(n)+sizeof(DWORD)-1)&(~(sizeof(DWORD)-1)))
#define VA_START(ap, arg) (ap = (VA_LIST)&arg + _INTSIZEOF(arg))
#define VA_END(ap) (ap=(VA_LIST)0);
#define VA_ARG(ap, type) (*(type*)((ap+=_INTSIZEOF(type))-_INTSIZEOF(type)))

//数据结构
#define ARV_QUEUE(buffer_size,type) struct{DWORD front;DWORD end;type buffer[buffer_size];}

//GDT/IDT描述符
typedef struct DESCRIPTOR_s{
	UWORD limit_low;
	UWORD base_low;
	UBYTE base_mid;
	UBYTE attr_first;
	UBYTE limit_high_attr_sec;
	UBYTE base_high;
} DESCRIPTOR;			

//门描述符
typedef struct GATE_s{
	UWORD offset_low;
	UWORD selector;
	UBYTE dcount;
	UBYTE attr;
	UWORD offset_high;
} GATE;

//常用常量
#define TRUE 1
#define FALSE 0
#define NULL 0
#define NULLPTR 0
#define CONST const
#define PUBLIC
#define STATIC static
#define PRIVATE STATIC


//GDT和IDT常量
#define GDT_SIZE 128
#define GDTR_SIZE 6
#define IDT_SIZE 256
#define IDTR_SIZE 6
#define LDT_SIZE 2
#define LDTR_SIZE 6

#define SELECTOR_KERNEL_CS 8

//CGA常量
#define CGA_DEFAULT_COLOR_W 0x3f //默认CGA颜色
#define CGA_WARNING_COLOR_W 0x0E //警告CGA颜色
#define CGA_EXCEPTION_COLOR_W 0x0C //异常CGA颜色

//VESA FONT
#define VESA_FONT_ROWS 19
#define VESA_FONT_COLS 12

#define VESA_RES_H 768
#define VESA_RES_W 1024

#define VESA_FONT_COLMAX (VESA_RES_W/VESA_FONT_COLS-1)



//其他
#define KCEX_PRINTF_BUFFERSIZE 1000
#define KCEX_MATH_TAYLOR_LIM 40
#define KCEX_MATH_DBL_EPS 1e-7
#define KCEX_MATH_FL_EPS 1e-5

//C-Style
#define M_PI 3.14159265358979323846
#define M_E 2.718281828459

//---------------------内核权限-----------------------------
#define KRNL_PRIVL_R0 0x0
#define KRNL_PRIVL_R1 0x1
#define KRNL_PRIVL_R2 0x2
#define KRNL_PRIVL_R3 0x3

#define KRNL_PRIVL_SYS KRNL_PRIVL_R0
#define KRNL_PRIVL_USR KRNL_PRIVL_R3
#define KRNL_PRIVL_TSK KRNL_PRIVL_R1

#define KRNL_RPL0 0x0
#define KRNL_RPL1 0x1
#define KRNL_RPL2 0x2
#define KRNL_RPL3 0x3

#define KRNL_RPL_SYS KRNL_RPL0
#define KRNL_RPL_USR KRNL_RPL3
#define KRNL_RPL_TSK KRNL_RPL1
//---------------------内联汇编-----------------------------
#define GCCASM_INTEL_SYNTAX asm(".intel_syntax noprefix");
#define GCCASM_ATT_SYNTAX asm(".att_syntax prefix");
#define CRLF ""
//---------------------Loader Selector----------------------
#define KRNL_LSELECTOR_EMPTY 0x0
#define KRNL_LSELECTOR_GENERAL 0x8
#define KRNL_LSELECTOR_GENERALDATA 0x10
#define KRNL_LSELECTOR_CGA 0x18 //Deprecated
#define KRNL_LSELECTOR_MDA 0x20 //Deprecated
#define KRNL_LSELECTOR_VGA 0x28 //Deprecated
#define KRNL_LSELECTOR_VESA 0x30

#define KRNL_LSELECTOR_VIDEO KRNL_LSELECTOR_VESA

#define KRNL_LSELECTOR_W_BEGIN 0x50
#define KRNL_LSELECTOR_TSS (KRNL_LSELECTOR_W_BEGIN+0x8*1)
#define KRNL_LSELECTOR_NXT (KRNL_LSELECTOR_W_BEGIN+0x8*2)


//---------------------IDT/GDT/LDT处理----------------------
typedef struct s_SELECTOR_S {
	DWORD selector_offset;
	DWORD table_indicator;
	DWORD rpl;
}SELECTOR_S;

#define KRNL_DA1(type, privl) ((type)|(privl<<5))
#define KRNL_LDSREG(selector,ti,rpl) (((selector)&(~7))|(ti)|(rpl)) //段寄存器描述(保护模式)
#define KRNL_LDSREG_S(selector_s) (((selector_s.selector_offset)&(~7))|(selector_s.table_indicator)|(selector_s.rpl))

#define KRNL_DESCRIPTOR_ATTR_386IGate 0x8E //386中断门
#define	KRNL_DESCRIPTOR_ATTR_C 0x98
#define	KRNL_DESCRIPTOR_ATTR_DRW 0x92
#define	KRNL_DESCRIPTOR_ATTR_LDT 0x82
#define	KRNL_DESCRIPTOR_ATTR_386TSS 0x89

#define KRNL_SELECTOR_TL_GDT 0x0 //TL:查询GDT表
#define KRNL_SELECTOR_TL_LDT 0x4 //TL:查询IDT表

#define KRNL_SELECTORS_USRL(selector) ((SELECTOR_S){(DWORD)selector,KRNL_SELECTOR_TL_LDT,KRNL_RPL_USR})
#define KRNL_SELECTORS_SYSG(selector) ((SELECTOR_S){(DWORD)selector,KRNL_SELECTOR_TL_GDT,KRNL_RPL_SYS})
#define KRNL_SELECTORS_SYSL(selector) ((SELECTOR_S){(DWORD)selector,KRNL_SELECTOR_TL_LDT,KRNL_RPL_SYS})
#define KRNL_SELECTORS_USRG(selector) ((SELECTOR_S){(DWORD)selector,KRNL_SELECTOR_TL_GDT,KRNL_RPL_USR})
#define KRNL_SELECTORS_TSKG(selector) ((SELECTOR_S){(DWORD)selector,KRNL_SELECTOR_TL_GDT,KRNL_RPL_TSK})
#define KRNL_SELECTORS_TSKL(selector) ((SELECTOR_S){(DWORD)selector,KRNL_SELECTOR_TL_LDT,KRNL_RPL_TSK})


//----------------------中断处理----------------------------

//中断向量
#define KRNL_INT_VECTOR_IRQ0 0x20
#define KRNL_INT_VECTOR_IRQ8 0x28

#define	KRNL_INT_VECTOR_DIVIDE		0x0
#define	KRNL_INT_VECTOR_DEBUG		0x1
#define	KRNL_INT_VECTOR_NMI			0x2
#define	KRNL_INT_VECTOR_BREAKPOINT		0x3
#define	KRNL_INT_VECTOR_OVERFLOW		0x4
#define	KRNL_INT_VECTOR_BOUNDS		0x5
#define	KRNL_INT_VECTOR_INVAL_OP		0x6
#define	KRNL_INT_VECTOR_COPROC_NOT		0x7
#define	KRNL_INT_VECTOR_DOUBLE_FAULT		0x8
#define	KRNL_INT_VECTOR_COPROC_SEG		0x9
#define	KRNL_INT_VECTOR_INVAL_TSS		0xA
#define	KRNL_INT_VECTOR_SEG_NOT		0xB
#define	KRNL_INT_VECTOR_STACK_FAULT		0xC
#define	KRNL_INT_VECTOR_PROTECTION		0xD
#define	KRNL_INT_VECTOR_PAGE_FAULT		0xE
#define	KRNL_INT_VECTOR_COPROC_ERR		0x10

//8259A
#define KRNL_INT_M_CTL 0x20
#define KRNL_INT_M_CTLMASK 0x21
#define KRNL_INT_S_CTL 0xA0
#define KRNL_INT_S_CTLMASK 0xA1

//中断请求
typedef VOID(*IRQ_HANDLER)(DWORD argv);
#define KRNL_INT_IRQ_COUNTS 16

#define KRNL_INT_IRQI_CLOCK 0x0
#define KRNL_INT_IRQI_KEYBOARD 0x1

//----------------------中断提示----------------------------
#define KRNL_INTTIPS_DE "#DE: Division by zero."
#define KRNL_INTTIPS_DB "#DB: Debug interrupt."
#define KRNL_INTTIPS_NMI "#NMI: Non maskable interrupt."
#define KRNL_INTTIPS_BP "#BP: Breakpoint triggered."
#define KRNL_INTTIPS_OF "#OF: Overflow."
#define KRNL_INTTIPS_BR "#BR: Bound range exceeded."
#define KRNL_INTTIPS_UD "#UD: Opreation code undefined."
#define KRNL_INTTIPS_NM "#NM: Device not available."
#define KRNL_INTTIPS_DF "#NM: Double fault."
#define KRNL_INTTIPS_MF "#MF: Coprocessor segment overrun"
#define KRNL_INTTIPS_TS "#TS: Invalid TSS"
#define KRNL_INTTIPS_NP "#NP: Segment not present"
#define KRNL_INTTIPS_SS "#SS: Stack corruption"
#define KRNL_INTTIPS_GP "#GP: General Protection"
#define KRNL_INTTIPS_PF "#PF: Page fault"
#define KRNL_INTTIPS_MF2 "#MF: Floating-point error"
#define KRNL_INTTIPS_AC "#AC: Alignment check"
#define KRNL_INTTIPS_UNKNOWN "#UN: Unknown Error"

//----------------------进程控制----------------------------
#define KRNL_PROC_NAME_LEN 16
#define KRNL_PROC_MAXCNT 4
#define KRNL_PROC_MAXTASKCNT KRNL_PROC_MAXCNT

#define KRNL_PROC_SINGLESTACK 5000
#define KRNL_PROC_TOTALSTACK (KRNL_PROC_MAXCNT*KRNL_PROC_SINGLESTACK)

#define KRNL_PROC_RINGPRIV_SYS 0
#define KRNL_PROC_RINGPRIV_USR 3
#define KRNL_PROC_RINGPRIV_TSK 1

#define KRNL_PROC_TTY_NULL -1


typedef UDWORD PROC_PID;
typedef struct s_stackframe {
	UDWORD	gs;		
	UDWORD	fs;		
	UDWORD	es;		
	UDWORD	ds;		
	UDWORD	edi;		
	UDWORD	esi;		
	UDWORD	ebp;		
	UDWORD	kernel_esp;	
	UDWORD	ebx;		
	UDWORD	edx;		
	UDWORD	ecx;		
	UDWORD	eax;		
	UDWORD	retaddr;
	UDWORD	eip;		
	UDWORD	cs;		
	UDWORD	eflags;		
	UDWORD	esp;	
	UDWORD	ss;		
}STACK_FRAME;
;

typedef struct s_task_struct {
	STACK_FRAME regs;
	SELECTOR_W ldt_selector;
	DESCRIPTOR ldt[LDT_SIZE];
	DWORD remaining_ticks;
	DWORD priority;
	PROC_PID pid;
	CHAR proc_name[KRNL_PROC_NAME_LEN];

	DWORD tty_id;
	DWORD privilege;
}PROCESS;

typedef struct s_task {
	HANDLER task_eip;
	DWORD stack_size;
	DWORD priority;
	CHAR name[32];
	UBYTE* stack_ptr;
	UBYTE privilege;
}TASK;

typedef struct s_tss {
	UDWORD	backlink;
	UDWORD	esp0;	
	UDWORD	ss0;
	UDWORD	esp1;
	UDWORD	ss1;
	UDWORD	esp2;
	UDWORD	ss2;
	UDWORD	cr3;
	UDWORD	eip;
	UDWORD	flags;
	UDWORD	eax;
	UDWORD	ecx;
	UDWORD	edx;
	UDWORD	ebx;
	UDWORD	esp;
	UDWORD	ebp;
	UDWORD	esi;
	UDWORD	edi;
	UDWORD	es;
	UDWORD	cs;
	UDWORD	ss;
	UDWORD	ds;
	UDWORD	fs;
	UDWORD	gs;
	UDWORD	ldt;
	UWORD	trap;
	UWORD	iobase;
}TSS;

//----------------------系统调用----------------------------
typedef VOID* SYSCALL;
#define KRNL_SYSCALL_COUNTS 4
#define KRNL_SYSCALL_IDX_GETTICK 0x0
#define KRNL_SYSCALL_IDX_CONWRITE 0x1
#define KRNL_SYSCALL_IDX_SENDREC 0x2 //Send/Receive原语

#define KRNL_SYSCALL_VEC 0x90


#define KRNL_SYSCALL_VEC_GETTICK 0x90


//----------------------计时器------------------------------
#define KRNL_HARDWARE_TIMERMODE 0x43
#define KRNL_HARDWARE_TIMER0 0x40
#define KRNL_HARDWARE_TIMER_RATEGEN 0x34
#define KRNL_HARDWARE_TIMER_FREQ 1193182L
#define KRNL_HARDWARE_TIMER_HZ 100

//----------------------扫描码------------------------------
#define KRNL_HW_KB_SCANCODE_CNT = 0x7F
#define KRNL_HW_KB_SCANCODE_COL = 0x03
#define KRNL_KB_FLAG_EXT 0x0100


#define KRNL_KB_PAD_ENTER (0x30 + KRNL_KB_FLAG_EXT)

#define KRNL_KB_FLAG_BREAK	0x0080		/* Break Code			*/
#define KRNL_KB_FLAG_EXT2	0x0100		/* Normal function keys		*/
#define KRNL_KB_FLAG_SHIFT_L	0x0200		/* Shift key			*/
#define KRNL_KB_FLAG_SHIFT_R	0x0400		/* Shift key			*/
#define KRNL_KB_FLAG_CTRL_L	0x0800		/* Control key			*/
#define KRNL_KB_FLAG_CTRL_R	0x1000		/* Control key			*/
#define KRNL_KB_FLAG_ALT_L	0x2000		/* Alternate key		*/
#define KRNL_KB_FLAG_ALT_R	0x4000		/* Alternate key		*/
#define KRNL_KB_FLAG_PAD	0x8000		/* keys in num pad		*/

#define KRNL_KB_ESC		(0x01 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_TAB		(0x02 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_ENTER		(0x03 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_BACKSPACE	(0x04 + KRNL_KB_FLAG_EXT)	

#define KRNL_KB_GUI_L		(0x05 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_GUI_R		(0x06 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_APPS		(0x07 + KRNL_KB_FLAG_EXT)	


#define KRNL_KB_SHIFT_L		(0x08 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_SHIFT_R		(0x09 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_CTRL_L		(0x0A + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_CTRL_R		(0x0B + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_ALT_L		(0x0C + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_ALT_R		(0x0D + KRNL_KB_FLAG_EXT)	


#define KRNL_KB_CAPS_LOCK	(0x0E + KRNL_KB_FLAG_EXT)	
#define	KRNL_KB_NUM_LOCK	(0x0F + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_SCROLL_LOCK	(0x10 + KRNL_KB_FLAG_EXT)	


#define KRNL_KB_F1		(0x11 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F2		(0x12 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F3		(0x13 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F4		(0x14 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F5		(0x15 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F6		(0x16 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F7		(0x17 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F8		(0x18 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F9		(0x19 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F10		(0x1A + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F11		(0x1B + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_F12		(0x1C + KRNL_KB_FLAG_EXT)	


#define KRNL_KB_PRINTSCREEN	(0x1D + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAUSEBREAK	(0x1E + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_INSERT		(0x1F + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_DELETE		(0x20 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_HOME		(0x21 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_END		(0x22 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAGEUP		(0x23 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAGEDOWN	(0x24 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_UP		(0x25 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_DOWN		(0x26 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_LEFT		(0x27 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_RIGHT		(0x28 + KRNL_KB_FLAG_EXT)	


#define KRNL_KB_POWER		(0x29 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_SLEEP		(0x2A + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_WAKE		(0x2B + KRNL_KB_FLAG_EXT)	


#define KRNL_KB_PAD_SLASH	(0x2C + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_STAR	(0x2D + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_MINUS	(0x2E + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_PLUS	(0x2F + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_ENTER	(0x30 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_DOT		(0x31 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_0		(0x32 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_1		(0x33 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_2		(0x34 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_3		(0x35 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_4		(0x36 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_5		(0x37 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_6		(0x38 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_7		(0x39 + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_8		(0x3A + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_9		(0x3B + KRNL_KB_FLAG_EXT)	
#define KRNL_KB_PAD_UP		KRNL_KB_PAD_8			
#define KRNL_KB_PAD_DOWN	KRNL_KB_PAD_2			
#define KRNL_KB_PAD_LEFT	KRNL_KB_PAD_4			
#define KRNL_KB_PAD_RIGHT	KRNL_KB_PAD_6			
#define KRNL_KB_PAD_HOME	KRNL_KB_PAD_7			
#define KRNL_KB_PAD_END		KRNL_KB_PAD_1			
#define KRNL_KB_PAD_PAGEUP	KRNL_KB_PAD_9			
#define KRNL_KB_PAD_PAGEDOWN	KRNL_KB_PAD_3			
#define KRNL_KB_PAD_INS		KRNL_KB_PAD_0			
#define KRNL_KB_PAD_MID		KRNL_KB_PAD_5			
#define KRNL_KB_PAD_DEL		KRNL_KB_PAD_DOT			

#define KRNL_KB_MASK_RAW	0x01ff

//----------------------键盘--------------------------------
#define KB_BUFFER_CAPACITY 0x200
typedef ARV_QUEUE(KB_BUFFER_CAPACITY, CHAR) KB_BUFFER;

//----------------------VESA--------------------------------
typedef struct VESA_PIXEL_S {
	UBYTE B;
	UBYTE G;
	UBYTE R;
}VESA_PIXEL;

#define KRNLM_VESA_DeclFrameBuffer(x) VESA_PIXEL x[VESA_RES_H * VESA_RES_W];
#define KRNLM_VESA_DeclFrameBufferArr(x, s) VESA_PIXEL x[VESA_RES_H * VESA_RES_W*s];
typedef VESA_PIXEL* VESA_FRAMEBUFFER;

//----------------------显示--------------------------------
typedef VESA_FRAMEBUFFER DISPLAY_BUFFER;

//----------------------控制台------------------------------
#define KRNL_TTY_BUF_SIZE 1024
#define KRNL_CON_COUNT 2
#define KRNL_CON_OUTPUT_BUFSIZE  KRNL_TTY_BUF_SIZE

typedef struct CONSOLE_S {
	DWORD ch_row;
	DWORD ch_col;
	DISPLAY_BUFFER disp_buf;
}CONSOLE;

typedef struct TTY_S {
	ARV_QUEUE(KRNL_TTY_BUF_SIZE, UDWORD) input_buffer;
	UDWORD input_buffer_cursize;

	CONSOLE* bound_con;
}TTY;

//----------------------进程间通信--------------------------
struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};
struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	UQWORD	m3l1;
	UQWORD	m3l2;
	void* m3p1;
	void* m3p2;
};
typedef struct {
	int source;
	int type;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
} MESSAGE;
#define KRNL_SNDREC_SEND 1
#define KRNL_SNDREC_RECEIVE 2
#define KRNL_SNDREC_BOTH 3
#define KRNL_SNDREC_INTERRUPT -10
#define KRNL_SNDREC_ANY (KRNL_PROC_MAXCNT+10)

#define KRNL_MAG_CH_PANIC '\002'
#define KRNL_MAG_CH_ASSERT '\003'