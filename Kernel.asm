; ---------------------------------------------------------------			
; Kernel.asm
; ---------------------------------------------------------------	
; Operating System Kernel
; OS�ں�
; By Aeroraven, 2021-3-16
; --------------------------------------------------------------

extern kernel_start

[SECTION .text]

global _start

_start: 
	xchg bx,bx
	call kernel_start
	jmp $
	