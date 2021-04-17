; ---------------------------------------------------------------			
; Kernel_AsmFunc.asm
; ---------------------------------------------------------------	
; Operating System Kernel - Assembly Functions Definitions
; OS�ں� - ��ຯ������
; By Aeroraven, 2021-4-3
; --------------------------------------------------------------

;MACROS
; ʹ��SaveRegsʱС��ջ���
%macro SaveRegs 0
	push ax
	push bx
	push cx
	push dx
	push si
	push di
	push bp
%endmacro

;�ָ��Ĵ���ֵ
%macro RecoverRegs 0
	pop bp
	pop di
	pop si
	pop dx
	pop cx
	pop bx
	pop ax
%endmacro

;�ݴ�Ĵ���ֵ32
%macro SaveRegs32 0
	push eax
	push ebx
	push ecx
	push edx
	push esi
	push edi
	push ebp
%endmacro

;�ָ��Ĵ���ֵ32
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

;��������
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

;���� AF_MemoryCopy [����ģʽ] - �ڴ渴��
;������	PUSH (dd)Ŀ��ָ��
;		PUSH (dd)��Դָ��
;		PUSH (dd)���ƴ�С
;		ES	 ���ݶ�ѡ����
;		DS   ���ݶ�ѡ����
;����:	EAX  0
;C��		DWORD AF_MemoryCopy(DWORD destPtr,DWORD sourcePtr,DWORD size)
AF_MemoryCopy:
	push ebp
	mov ebp,esp
	push esi
	push edi
	push ecx
	mov edi,[ebp+8] ;DEST, ǰ�滹��EIP��EBP
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

;���� AF_LoadGlobalDescriptorTable [����ģʽ] - ����GDTR
;������	PUSH (dd)GDTRָ��
;		DS   ���ݶ�ѡ����
;����:	EAX  0
;C��		DWORD AF_LoadGlobalDescriptorTable(DWORD gdtPtr)
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

;���� AF_SaveGlobalDescriptorTable [����ģʽ] - ����GDTR
;������	PUSH (dd)GDTRָ��
;		DS   ���ݶ�ѡ����
;����:	EAX  0
;C��		DWORD AF_SaveGlobalDescriptorTable(DWORD gdtPtr)
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


;���� AF_VMBreakPoint [����ģʽ] - ������ϵ�(0����β)
AF_VMBreakPoint:
	push ebp
	mov ebp,esp
	xchg bx,bx
	pop ebp
	ret


;���� AF_DispChar [����ģʽ] - ��ʾ�ַ�
;������ 
;		PUSH (dd) �ַ�
;		PUSH (dd) ��ʽ
;		PUSH (dd) λ��
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

;���� AF_DispChar [����ģʽ] - �������λ��
AF_GetDispPos:
	push ebp
	mov ebp,esp
	mov esi,[ebp+8]
	mov edx,[DSPPOS]
	mov [ds:esi],edx
	mov eax,[DSPPOS]
	pop ebp
	ret

;���� AF_DispChar [����ģʽ] - �������λ��
AF_SetDispPos:
	push ebp
	mov ebp,esp
	mov eax,[ebp+8]
	mov dword [DSPPOS],eax
	pop ebp
	ret

;����AF_OutPort [����ģʽ] - �˿����
AF_OutPort:
	mov edx, [esp+4]
	mov al,[esp+8]
	out dx,al
	nop
	nop
	ret

;����AF_InPort [����ģʽ] - �˿�����
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