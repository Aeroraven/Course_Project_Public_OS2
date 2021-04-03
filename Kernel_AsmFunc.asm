; ---------------------------------------------------------------			
; Kernel_AsmFunc.asm
; ---------------------------------------------------------------	
; Operating System Kernel - Assembly Functions Definitions
; OS内核 - 汇编函数定义
; By Aeroraven, 2021-4-3
; --------------------------------------------------------------

;MACROS
; 使用SaveRegs时小心栈溢出
%macro SaveRegs 0
	push ax
	push bx
	push cx
	push dx
	push si
	push di
	push bp
%endmacro

;恢复寄存器值
%macro RecoverRegs 0
	pop bp
	pop di
	pop si
	pop dx
	pop cx
	pop bx
	pop ax
%endmacro

;暂存寄存器值32
%macro SaveRegs32 0
	push eax
	push ebx
	push ecx
	push edx
	push esi
	push edi
	push ebp
%endmacro

;恢复寄存器值32
%macro RecoverRegs32 0
	pop ebp
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
%endmacro

;---

[SECTION .data]
DBP_DISPLAY_PRINTLINE1		db 0
DBP_DISPLAY_PRINTCOL1		db 0
CONST_VIDEO_CGA_COLS		equ 80

[SECTION .text]

;导出函数
global AF_MemoryCopy
global AF_LoadGlobalDescriptorTable
global AF_DispStrCGA

;函数 AF_MemoryCopy [保护模式] - 内存复制
;参数：	PUSH (dd)目标指针
;		PUSH (dd)来源指针
;		PUSH (dd)复制大小
;		ES	 数据段选择子
;		DS   数据段选择子
;返回:	EAX  0
;C：		DWORD AF_MemoryCopy(DWORD destPtr,DWORD sourcePtr,DWORD size)
AF_MemoryCopy:
	push ebp
	mov ebp,esp
	push esi
	push edi
	push ecx
	mov edi,[ebp+8] ;DEST, 前面还有EIP和EBP
	mov esi,[ebp+12];SRC
	mov ecx,[ebp+16]
	
.1:
	mov al, [ds:esi]
	inc esi
	mov byte [es:edi],al
	inc edi
	loop .1
	mov eax,[ebp+8]
	pop ecx
	pop esi
	pop edi
	mov esp,ebp
	pop ebp
	ret 

;函数 AF_LoadGlobalDescriptorTable [保护模式] - 加载GDTR
;参数：	PUSH (dd)GDTR指针
;		DS   数据段选择子
;返回:	EAX  0
;C：		DWORD AF_LoadGlobalDescriptorTable(DWORD gdtPtr)
AF_LoadGlobalDescriptorTable:
	push ebp
	mov ebp,esp
	push esi
	mov esi,[ebp+8]
	lgdt [ds:esi]
	pop esi
	mov esp,ebp
	pop ebp
	xor eax,eax
	ret

;函数 AF_DispStrCGA [保护模式] - 显示字符串(0作结尾)
;参数： AH 颜色常量
;		ES 数据段选择子
;		PUSH (dd) 字符串地址
AF_DispStrCGA:
	xchg bx,bx
	push ebp
	mov ebp,esp
	push esi
	;计算EDI
	xchg bx,bx
	push eax
	mov eax,[ebp+8]
	mov esi,eax
	xor eax,eax
	mov al,[DBP_DISPLAY_PRINTLINE1]
	mov bx,CONST_VIDEO_CGA_COLS
	mul bx
	xor cx,cx
	mov cl,[DBP_DISPLAY_PRINTCOL1]
	add ax,cx
	add eax,eax
	mov edi,eax
	pop eax
	mov dx,0
.DISPCGA_1:
	mov al,[ds:esi]
	cmp al,0
	jz .DISPCGA_2
	mov [gs:edi],ax
	inc dx
	inc esi
	add edi,2
	jmp .DISPCGA_1
.DISPCGA_2:
	mov [DBP_DISPLAY_PRINTCOL1],dl
	pop esi
	mov esp,ebp
	pop ebp
	xchg bx,bx
	ret