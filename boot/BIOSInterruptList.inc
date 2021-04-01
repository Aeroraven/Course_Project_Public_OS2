; ---------------------------------------------------------------			
; BIOSInterruptList.inc
; ---------------------------------------------------------------			
; Macros for BIOS Interrupt Calls (x86)
; x86 BIOS �жϵ��ú�
; By Aeroraven, 2021-3-12
; --------------------------------------------------------------
; �ο�: https://www.cs.cmu.edu/~ralf/interrupt-list/inter61a.zip
; ---------------------------------------------------------------


;----------------------------------			
; ��ɫ����
;----------------------------------
CONST_BINT_BLACK_FG			equ 0x0
CONST_BINT_BLUE_FG			equ 0x1
CONST_BINT_GREEN_FG			equ 0x2
CONST_BINT_RED_FG			equ 0x4
CONST_BINT_INTENSITY_FG		equ 0x8
CONST_BINT_WHITE_FG			equ 0xF

CONST_BINT_BLACK_BG			equ 0x0
CONST_BINT_BLUE_BG			equ 0x10
CONST_BINT_GREEN_BG			equ 0x20
CONST_BINT_RED_BG			equ 0x40
CONST_BINT_INTENSITY_BG		equ 0x80
CONST_BINT_WHITE_BG			equ 0xF0

;----------------------------------			
; �ж� 10h: ��ʾ����
;----------------------------------

; [INT10h/00] : ������ʾģʽ
; ����: VideoMode
; ����: NULL
%macro INT10_SetVideoMode 1
	mov ah,00h
	mov al,%1
	int 10h
%endmacro


; [INT10h/01] : ���ù����״
; ����: �� (4bit)
;		�� (4bit)
; ����: NULL
%macro INT10_SetCursorShape 2
	mov ah,01h
	mov ch,%1
	mov cl,%2
	int 10h
%endmacro


; [INT10h/02] : ���ù��λ�� (Text Mode)
; ����: ҳ�� 
;		�� 
;		�� 
; ����: NULL
%macro INT10_SetCursorPos 3
	mov ah,02h
	mov bh,%1
	mov ch,%2
	mov cl,%3
	int 10h
%endmacro

;[INT10h/03]: ��ȡ���λ��
; ������BH ҳ��
; ���أ�DH ��
;		DL ��
%macro INT10_GetCursorPos 1
	mov bh,%1
	int 10h
%endmacro

; [INT10h/06] : ���Ϲ���
; ����: �������� (0 - ����)
;		�������
;		��������
;		��������
; ����: NULL
%macro INT10_ScrollUp 4
	mov ax,(0600h | %1)
	mov bh,%2
	mov cx,%3
	mov dx,%4
	int 10h
%endmacro

; [INT10h/07] : ���¹���
; ����: �������� (0 - ����)
;		�������
;		��������
;		��������
; ����: NULL
%macro INT10_ScrollDown 4
	mov ah,07h
	mov al,%1
	mov bh,%2
	mov cx,%3
	mov dx,%4
	int 10h
%endmacro

; [INT10h/0F] : ��ȡ��Ƶģʽ
; ����: NULL
; ����: AL ��Ƶģʽ
;		AH �ַ�����
;		BH ���ʾҳ��
%macro INT10_GetVideoMode 0
	mov ah,0fh
	int 10h
%endmacro

; [INT10h/0C] : ���ͼ������
; ����: AL	��ɫ
;		BH	��ʾҳ��
;		CX	X����
;		DX	Y����
; ����: NULL
%macro INT10_WrPixel 4
	mov al, %1
	mov bh, %2
	mov cx, %3
	mov dx, %4
	mov ah, 0Ch
	int 10h
%endmacro


; [INT10h/13] : ��ʾ�ַ���(TeletypeMode)
; ����: ҳ��
;		��ʾ�����ʽ
;		����
;		x�ַ�������
;		x����
; ����: ES:BP �ַ�����ʼ��ַ
;		DX ����(DH,DL)
;		CX ����
; ����: NULL
%macro INT10_DispStr 3
	mov ax,(1300h | %2)
	mov bx,(%1<<16)|%3
	int 10h
%endmacro

;----------------------------------			
; �ж� 13h: �ͼ�Ӳ���豸����
;----------------------------------

; [INT13h/00] : ����Ӳ���豸
; ����: Ӳ�̺�
%macro INT13_RstDskSys 1
	mov ah, 00h
	mov dl, %1
	int 13h
%endmacro

; [INT13h/02] : ��ȡ����
; ����: ��ȡ������, 
;		�����, 
;		��ʼ������, 
;		��ͷ��, 
;		Ӳ�̺�
; Ԥ��: ES:BX �����ַ
%macro INT13_RdSector 5
	mov ch,%2
	mov cl,%3
	mov dh,%4
	mov dl,%5
	mov ah,02h
	mov al,%1
	int 13h
%endmacro

; [INT13h/02] : ��ȡ����(��������������)
; ����: ��ȡ������ AL, 
;		����� CH, 
;		��ʼ������ CL, 
;		��ͷ�� DH, 
;		Ӳ�̺� DL
; Ԥ��: ES:BX �����ַ
%macro INT13_RdSector2 5
	mov ch,%2
	mov cl,%3
	mov dh,%4
	mov dl,%5
%%INT13_RdSector_Retry:
	mov ah,02h
	mov al,%1
	int 13h
	jc %%INT13_RdSector_Retry
%endmacro

; [INT13h/02] : ��ȡ����(��������������)
; ����: 	��ȡ������
; Ԥ��: ES:BX �����ַ
%macro INT13_RdSector3 1
%%INT13_RdSector_Retry3:
	mov ah,02h
	mov al,%1
	;xchg bx,bx
	int 13h
	;xchg bx,bx
	jc %%INT13_RdSector_Retry3
%endmacro

;----------------------------------			
; �ж� 15h: ����
;----------------------------------

;[INT15/E8-20] ����ѯϵͳ��ַӳ��
; ����:	EBX ����ֵ(��ʼ��ѯΪ0)
;		ECX ��������С
; ����: ES:DI ��������ַ
; ���:	CF	Ϊ���ʾ�޴���
;		EAX	ǩ��SMAP
;		ECX ��������С
;		EBX ����ֵ (0��CF��ʾ���һ��ARD)
;		[ES:DI]	ARD����
%macro INT15_QuerySMAP 2
	mov eax, 0e820h
	mov edx, 0534d4150h
	mov ebx, %1
	mov ecx, %2
	int 15h
%endmacro