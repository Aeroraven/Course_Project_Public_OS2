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

EXTERN UBYTE KRNL_KB_CtrlL;
EXTERN UBYTE KRNL_KB_CtrlR;
EXTERN UBYTE KRNL_KB_AltL;
EXTERN UBYTE KRNL_KB_AltR;
EXTERN UBYTE KRNL_KB_ShiftL;
EXTERN UBYTE KRNL_KB_ShiftR;
EXTERN UBYTE KRNL_KB_E0_Flag;
EXTERN UBYTE KRNL_KB_NumLock;
EXTERN UBYTE KRNL_KB_CapLock;
EXTERN UBYTE KRNL_KB_ScrollLock;

EXTERN UBYTE KRNL_CON_CurConsole;
EXTERN KB_BUFFER kb_buffer;

//extern KRNLM_VESA_DeclFrameBuffer(KRNL_VESA_FrameBuffer);

extern KRNLM_VESA_DeclFrameBuffer(KRNL_CON_VFrameBuffer_0);
extern KRNLM_VESA_DeclFrameBuffer(KRNL_CON_VFrameBuffer_1);

#define KRNL_VESA_FrameBuffer KRNL_CON_VFrameBuffer_0

EXTERN DISPLAY_BUFFER KRNL_DISP_ActivatedBuffer;

extern TTY KRNL_TTY_Table[KRNL_CON_COUNT];
extern CONSOLE KRNL_CON_Table[KRNL_CON_COUNT];

extern UWORD KRNL_KeyMap[0x180];
extern UBYTE KRNL_VIDEO_CHARMAP[128][VESA_FONT_COLS * VESA_FONT_ROWS];

extern PROCESS ProcessTable[KRNL_PROC_MAXCNT];

extern CHAR TestStack[KRNL_PROC_SINGLESTACK];
extern CHAR TestStack2[KRNL_PROC_SINGLESTACK];
extern CHAR TestStack3[KRNL_PROC_SINGLESTACK];
extern CHAR TestStack4[KRNL_PROC_SINGLESTACK];
extern CHAR TestStack5[KRNL_PROC_SINGLESTACK];

extern TSS tss;
extern PROCESS* ProcessReady;
extern DWORD TickCount;
extern TASK task_table[KRNL_PROC_MAXCNT];

extern IRQ_HANDLER irq_handler[KRNL_INT_IRQ_COUNTS];
extern SYSCALL syscall_table[KRNL_SYSCALL_COUNTS];