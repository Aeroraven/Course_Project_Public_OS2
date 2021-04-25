// ---------------------------------------------------------------			
// Kernel_GlobalVar.c
// ---------------------------------------------------------------	
// Operating System Kernel Global Variables
// OS内核 - 全局变量
// By Aeroraven, 2021-4-4
// --------------------------------------------------------------

#define GLOBAL_VARS 1

#include "Kernel_CDef.h"
#include "Kernel_GlobalVar.h"

PROCESS ProcessTable[KRNL_PROC_MAXCNT];

CHAR TestStack[8000];
CHAR TestStack2[8000];
CHAR TestStack3[8000];

TSS tss;
PROCESS* ProcessReady;
DWORD TickCount;

TASK task_table[KRNL_PROC_MAXCNT];

IRQ_HANDLER irq_handler[KRNL_INT_IRQ_COUNTS];
SYSCALL syscall_table[KRNL_SYSCALL_COUNTS];

UWORD KRNL_VIDEO_CHARMAP[128][VESA_FONT_COLS * VESA_FONT_ROWS];


UWORD KRNL_KeyMap[0x180]= {
		0, 0, 0,
		KRNL_KB_ESC, KRNL_KB_ESC, 0,
		'1', '!', 0,
		'2', '@', 0,
		'3', '#', 0,
		'4', '$', 0,
		'5', '%', 0,
		'6', '^', 0,
		'7', '&', 0,
		'8', '*', 0,
		'9', '(', 0,
		'0', ')', 0,
		'-', '_', 0,
		'=', '+', 0,
		KRNL_KB_BACKSPACE, KRNL_KB_BACKSPACE, 0,
		KRNL_KB_TAB, KRNL_KB_TAB, 0,
		'q', 'Q', 0,
		'w', 'W', 0,
		'e', 'E', 0,
		'r', 'R', 0,
		't', 'T', 0,
		'y', 'Y', 0,
		'u', 'U', 0,
		'i', 'I', 0,
		'o', 'O', 0,
		'p', 'P', 0,
		'[', '{', 0,
		']', '}', 0,
		KRNL_KB_ENTER, KRNL_KB_ENTER, KRNL_KB_PAD_ENTER,
		KRNL_KB_CTRL_L, KRNL_KB_CTRL_L, KRNL_KB_CTRL_R,
		'a', 'A', 0,
		's', 'S', 0,
		'd', 'D', 0,
		'f', 'F', 0,
		'g', 'G', 0,
		'h', 'H', 0,
		'j', 'J', 0,
		'k', 'K', 0,
		'l', 'L', 0,
		';', ':', 0,
		'\'', '"', 0,
		'`', '~', 0,
		KRNL_KB_SHIFT_L, KRNL_KB_SHIFT_L, 0,
		'\\', '|', 0,
		'z', 'Z', 0,
		'x', 'X', 0,
		'c', 'C', 0,
		'v', 'V', 0,
		'b', 'B', 0,
		'n', 'N', 0,
		'm', 'M', 0,
		',', '<', 0,
		'.', '>', 0,
		'/', '?', KRNL_KB_PAD_SLASH,
		KRNL_KB_SHIFT_R, KRNL_KB_SHIFT_R, 0,
		'*', '*', 0,
		KRNL_KB_ALT_L, KRNL_KB_ALT_L, KRNL_KB_ALT_R,
		' ', ' ', 0,
		KRNL_KB_CAPS_LOCK, KRNL_KB_CAPS_LOCK, 0,
		KRNL_KB_F1, KRNL_KB_F1, 0,
		KRNL_KB_F2, KRNL_KB_F2, 0,
		KRNL_KB_F3, KRNL_KB_F3, 0,
		KRNL_KB_F4, KRNL_KB_F4, 0,
		KRNL_KB_F5, KRNL_KB_F5, 0,
		KRNL_KB_F6, KRNL_KB_F6, 0,
		KRNL_KB_F7, KRNL_KB_F7, 0,
		KRNL_KB_F8, KRNL_KB_F8, 0,
		KRNL_KB_F9, KRNL_KB_F9, 0,
		KRNL_KB_F10, KRNL_KB_F10, 0,
		KRNL_KB_NUM_LOCK, KRNL_KB_NUM_LOCK, 0,
		KRNL_KB_SCROLL_LOCK, KRNL_KB_SCROLL_LOCK, 0,
		KRNL_KB_PAD_HOME, '7', KRNL_KB_HOME,
		KRNL_KB_PAD_UP, '8', KRNL_KB_UP,
		KRNL_KB_PAD_PAGEUP, '9', KRNL_KB_PAGEUP,
		KRNL_KB_PAD_MINUS, '-', 0,
		KRNL_KB_PAD_LEFT, '4', KRNL_KB_LEFT,
		KRNL_KB_PAD_MID, '5', 0,
		KRNL_KB_PAD_RIGHT, '6', KRNL_KB_RIGHT,
		KRNL_KB_PAD_PLUS, '+', 0,
		KRNL_KB_PAD_END, '1', KRNL_KB_END,
		KRNL_KB_PAD_DOWN, '2', KRNL_KB_DOWN,
		KRNL_KB_PAD_PAGEDOWN, '3', KRNL_KB_PAGEDOWN,
		KRNL_KB_PAD_INS, '0', KRNL_KB_INSERT,
		KRNL_KB_PAD_DOT, '.', KRNL_KB_DELETE,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		KRNL_KB_F11, KRNL_KB_F11, 0,
		KRNL_KB_F12, KRNL_KB_F12, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, KRNL_KB_GUI_L,
		0, 0, KRNL_KB_GUI_R,
		0, 0, KRNL_KB_APPS,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
};