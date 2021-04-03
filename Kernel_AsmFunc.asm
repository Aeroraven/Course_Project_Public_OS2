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

[SECTION .text]

;��������
global AF_MemoryCopy
global AF_LoadGlobalDescriptorTable
global AF_DispStrCGA

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
	mov esi,[ebp+8]
	lgdt [ds:esi]
	pop esi
	mov esp,ebp
	pop ebp
	xor eax,eax
	ret

;���� AF_DispStrCGA [����ģʽ] - ��ʾ�ַ���(0����β)
;������ AH ��ɫ����
;		ES ���ݶ�ѡ����
;		PUSH (dd) �ַ�����ַ
AF_DispStrCGA:
	xchg bx,bx
	push ebp
	mov ebp,esp
	push esi
	;����EDI
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