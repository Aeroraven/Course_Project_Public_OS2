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
EXTERN UDWORD K_IntReenter;
EXTERN UDWORD K_Ticks;

EXTERN UBYTE T[4];
EXTERN DESCRIPTOR GDT[GDT_SIZE];
EXTERN GATE IDT[IDT_SIZE];

EXTERN UDWORD KRNL_VESAFont_Col;
EXTERN UDWORD KRNL_VESAFont_Row;

extern UWORD KRNL_KeyMap[0x180];
extern UBYTE KRNL_VIDEO_CHARMAP[128][VESA_FONT_COLS * VESA_FONT_ROWS];

extern PROCESS ProcessTable[KRNL_PROC_MAXCNT];

extern CHAR TestStack[8000];
extern CHAR TestStack2[8000];
extern CHAR TestStack3[8000];
extern CHAR TestStack4[8000];

extern TSS tss;
extern PROCESS* ProcessReady;
extern DWORD TickCount;
extern TASK task_table[KRNL_PROC_MAXCNT];

extern IRQ_HANDLER irq_handler[KRNL_INT_IRQ_COUNTS];
extern SYSCALL syscall_table[KRNL_SYSCALL_COUNTS];