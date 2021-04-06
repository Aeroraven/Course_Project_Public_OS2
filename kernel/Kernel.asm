; ---------------------------------------------------------------			
; Kernel.asm
; ---------------------------------------------------------------	
; Operating System Kernel
; OS内核
; By Aeroraven, 2021-3-16
; --------------------------------------------------------------


extern kernel_start
extern exception_handler
extern spurious_interrupt_request
;宏
%macro  spurious_int_placeholder    1
        push    %1
        call    spurious_interrupt_request
        add     esp, 4
        hlt
%endmacro
;开始




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
	
;中断
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

global AFE_INT_0
global AFE_INT_1
global AFE_INT_2
global AFE_INT_3
global AFE_INT_4
global AFE_INT_5
global AFE_INT_6
global AFE_INT_7
global AFE_INT_8
global AFE_INT_9
global AFE_INT_10
global AFE_INT_11
global AFE_INT_12
global AFE_INT_13
global AFE_INT_14
global AFE_INT_15
global AFE_INT_16


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

AFE_INT_0:
	spurious_int_placeholder 0
AFE_INT_1:
	spurious_int_placeholder 1
AFE_INT_2:
	spurious_int_placeholder 2
AFE_INT_3:
	spurious_int_placeholder 3
AFE_INT_4:
	spurious_int_placeholder 4
AFE_INT_5:
	spurious_int_placeholder 5
AFE_INT_6:
	spurious_int_placeholder 6
AFE_INT_7:
	spurious_int_placeholder 7
AFE_INT_8:
	spurious_int_placeholder 8
AFE_INT_9:
	spurious_int_placeholder 9
AFE_INT_10:
	spurious_int_placeholder 10
AFE_INT_11:
	spurious_int_placeholder 11
AFE_INT_12:
	spurious_int_placeholder 12
AFE_INT_13:
	spurious_int_placeholder 13
AFE_INT_14:
	spurious_int_placeholder 14
AFE_INT_15:
	spurious_int_placeholder 15
AFE_INT_16:
	spurious_int_placeholder 16

exception_handle:
	call exception_handler
	add esp,8
	jmp $