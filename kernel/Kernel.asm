; ---------------------------------------------------------------			
; Kernel.asm
; ---------------------------------------------------------------	
; Operating System Kernel
; OS�ں�
; By Aeroraven, 2021-3-16
; --------------------------------------------------------------

extern kernel_start


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
	