; ---------------------------------------------------------------			
; Kernel.asm
; ---------------------------------------------------------------	
; Operating System Kernel
; OSÄÚºË
; By Aeroraven, 2021-3-16
; --------------------------------------------------------------

extern kernel_start
extern exception_handler


[SECTION .bss]
StackSpace		resb	2 * 1024
StackTop:		

[SECTION .text]

global _start

_start: 
	mov	esp, StackTop
	xchg bx,bx
	call kernel_start
	jmp $
	
;ÖÐ¶Ï
global AFE_EXCEPTION_DE
global AFE_EXCEPTION_DB
global AFE_EXCEPTION_NMI
global AFE_EXCEPTION_BP
global AFE_EXCEPTION_OF
global AFE_EXCEPTION_BR
global AFE_EXCEPTION_UD
global AFE_EXCEPTION_NM
global AFE_EXCEPTION_DF
global AFE_EXCEPTION_MF
global AFE_EXCEPTION_TS
global AFE_EXCEPTION_NP
global AFE_EXCEPTION_SS
global AFE_EXCEPTION_GP
global AFE_EXCEPTION_PF
global AFE_EXCEPTION_RSVD
global AFE_EXCEPTION_MF2
global AFE_EXCEPTION_AC
global AFE_EXCEPTION_MC
global AFE_EXCEPTION_XM

AFE_EXCEPTION_DE:
	push 0x0
	push 0
	jmp exception_handle
AFE_EXCEPTION_DB:
	push 0x0
	push 1
	jmp exception_handle
AFE_EXCEPTION_NMI:
	push 0x0
	push 2
	jmp exception_handle
AFE_EXCEPTION_BP:
	push 0x0
	push 3
	jmp exception_handle
AFE_EXCEPTION_OF:
	push 0x0
	push 4
	jmp exception_handle
AFE_EXCEPTION_BR:
	push 0x0
	push 5
	jmp exception_handle
AFE_EXCEPTION_UD:
	push 0x0
	push 6
	jmp exception_handle
AFE_EXCEPTION_NM:
	push 0x0
	push 7
	jmp exception_handle
AFE_EXCEPTION_DF:
	;push 0x0
	push 8
	jmp exception_handle
AFE_EXCEPTION_MF:
	push 0x0
	push 9
	jmp exception_handle
AFE_EXCEPTION_TS:
	;push 0x0
	push 10
	jmp exception_handle
AFE_EXCEPTION_NP:
	;push 0x0
	push 11
	jmp exception_handle
AFE_EXCEPTION_SS:
	;push 0x0
	push 12
	jmp exception_handle
AFE_EXCEPTION_GP:
	;push 0xffffffff
	push 13
	jmp exception_handle
AFE_EXCEPTION_PF:
	;push 0xffffffff
	push 14
	jmp exception_handle
AFE_EXCEPTION_RSVD:
	push 0x0
	push 15
	jmp exception_handle
AFE_EXCEPTION_MF2:
	push 0x0
	push 16
	jmp exception_handle
AFE_EXCEPTION_AC:
	push 0x0
	push 17
	jmp exception_handle
AFE_EXCEPTION_MC:
	push 0x0
	push 18
	jmp exception_handle
AFE_EXCEPTION_XM:
	push 0x0
	push 19
	jmp exception_handle


exception_handle:
	call exception_handler
	add esp,8
	jmp $