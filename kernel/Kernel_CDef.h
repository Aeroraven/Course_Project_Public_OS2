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

//类型定义
typedef unsigned int UDWORD; 		//双字，等效于unsigned int
typedef unsigned short UWORD;	//字，等效于unsigned short
typedef unsigned char UBYTE;		//字节，等效于unsigned char;

typedef int DWORD;		//双字，等效于int
typedef short WORD;		//字，等效于short
typedef char  BYTE;		//字节，等效于char

typedef void* ANYPTR;	//空类型指针,等效于UDWORD
typedef void* GDTPTR;	//GDT指针
typedef void* LDTPTR;	//LDT指针
typedef void* IDTPTR;   //IDT指针

typedef void VOID;
typedef char CHAR;

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

#define SELECTOR_KERNEL_CS 8

//CGA常量
#define CGA_DEFAULT_COLOR_W 0x3f //默认CGA颜色
#define CGA_WARNING_COLOR_W 0x0E //警告CGA颜色
#define CGA_EXCEPTION_COLOR_W 0x0C //异常CGA颜色

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


//---------------------IDT/GDT/LDT处理----------------------
#define KRNL_DESCRIPTOR_ATTR_386IGate 0x8E //386中断门


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