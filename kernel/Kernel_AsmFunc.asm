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
DDP_SGDT_ADDR				dd 0
DSPPOS					dd 0
[SECTION .text]

;导出函数
global AF_MemoryCopy
global AF_LoadGlobalDescriptorTable
global AF_LoadInterruptDescriptorTable
global AF_SaveGlobalDescriptorTable
global AF_VMBreakPoint
global AF_DispChar
global AF_GetDispPos
global AF_SetDispPos

global AF_OutPort
global AF_InPort

global AF_UD2Call
global AF_CLICall
global AF_STICall
global AF_HLTCall
global AF_LTRAxCall

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
	mov eax,[ebp+8]
	lgdt [eax]
	pop esi
	mov esp,ebp
	pop ebp
	xor eax,eax
	ret


AF_LoadInterruptDescriptorTable:
	push ebp
	mov ebp,esp
	push esi
	mov eax,[ebp+8]
	lidt [eax]
	pop esi
	mov esp,ebp
	pop ebp
	xor eax,eax
	ret

;函数 AF_SaveGlobalDescriptorTable [保护模式] - 储存GDTR
;参数：	PUSH (dd)GDTR指针
;		DS   数据段选择子
;返回:	EAX  0
;C：		DWORD AF_SaveGlobalDescriptorTable(DWORD gdtPtr)
AF_SaveGlobalDescriptorTable:
	push ebp
	mov ebp,esp
	push esi
	mov esi,[ebp+8]
	push eax
	mov eax,[ebp+8]
	sgdt [eax]
	pop eax
	pop esi
	mov esp,ebp
	pop ebp
	xor eax,eax
	ret


;函数 AF_VMBreakPoint [保护模式] - 虚拟机断点(0作结尾)
AF_VMBreakPoint:
	push ebp
	mov ebp,esp
	xchg bx,bx
	pop ebp
	ret


;函数 AF_DispChar [保护模式] - 显示字符
;参数： 
;		PUSH (dd) 字符
;		PUSH (dd) 样式
;		PUSH (dd) 位置
AF_DispChar:
	push ebp
	mov ebp,esp
	mov ebx,[DSPPOS]
	mov al,[ebp+8]
	mov ah,[ebp+12]
	mov edi,ebx

	push eax
	mov eax,dword[DSPPOS]
	cmp eax,8000
	pop eax
	ja AF_DispChar_Ignore
	

	;xchg bx,bx
	mov [gs:edi],ax
AF_DispChar_Ignore:
	pop ebp
	inc dword [DSPPOS]
	inc dword [DSPPOS]
	ret

;函数 AF_DispChar [保护模式] - 保存输出位置
AF_GetDispPos:
	push ebp
	mov ebp,esp
	mov esi,[ebp+8]
	mov edx,[DSPPOS]
	mov [ds:esi],edx
	mov eax,[DSPPOS]
	pop ebp
	ret

;函数 AF_DispChar [保护模式] - 设置输出位置
AF_SetDispPos:
	push ebp
	mov ebp,esp
	mov eax,[ebp+8]
	mov dword [DSPPOS],eax
	pop ebp
	ret

;函数AF_OutPort [保护模式] - 端口输出
AF_OutPort:
	mov edx, [esp+4]
	mov al,[esp+8]
	out dx,al
	nop
	nop
	ret

;函数AF_InPort [保护模式] - 端口输入
AF_InPort:
	mov edx, [esp+4]
	mov al,[esp+8]
	in al,dx
	nop
	nop
	ret

AF_UD2Call:
	ud2
	ret

AF_HLTCall:
	hlt
	ret

AF_INFLoopCall:
	jmp $
	ret

AF_CLICall:
	cli
	ret

AF_STICall:
	sti
	ret

AF_LTRAxCall:
	push ebp
	mov ebp,esp
	mov ax,[ebp+8]
	ltr ax
	pop ebp
	ret