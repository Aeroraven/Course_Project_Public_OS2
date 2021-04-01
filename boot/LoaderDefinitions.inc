; ---------------------------------------------------------------			
; LoaderDefinitions.asm
; ---------------------------------------------------------------	
; Boot Loader Definitions
; �����������Ͷ���
; By Aeroraven, 2021-3-13
; --------------------------------------------------------------
; �ο�: Forrest Yu
; --------------------------------------------------------------


;----------------------------------			
; ������(Descriptor)
;----------------------------------
%macro DESCRIPTOR 3 ;DESCRIPTOR Base,Limit,Attr
	dw (%2 & 0FFFFh)							;����0:15			(0-15λ)
	dw (%1 & 0FFFFh)							;��ַ0:15			(16-31λ)
	db (%1 >> 16) & 0FFh						;��ַ16:24			(32-39λ)
	db (%3 & 0FFFFh)							;�����ֽ�			(40-47λ)
	db ((%2 >> 16) & 00Fh )| ((%3 >> 4) & 0F0h)	;����16:19 + ��ʶ	(48-55λ)
	db (%1 >> 24) & 0FFh						;��ַ24:31			(56-63λ��
%endmacro

;�����ֽڽṹ: Ac,Rw,Dc,Ex,S,Privl,Present
;��ʶ�ṹ: 0,0,Sz,Gr

CONST_DA_AC_ACCESSED			equ 0x001		;Ac:�α�����
CONST_DA_AC_DEFAULT				equ 0x000		;Ac:Ĭ��ֵ

CONST_DA_RW_READONLY			equ 0x000		;Rw:��ֻ��
CONST_DA_RW_WRITABLE			equ 0x002		;Rw:�οɶ�д

CONST_DA_DCD_UP					equ 0x000		;Dc:����ѡ����-�η�������
CONST_DA_DCD_DOWN				equ 0x004		;Dc:����ѡ����-�η�������
CONST_DA_DCC_CONFORMING			equ 0x004		;Dc:�����ѡ����-һ�´����
CONST_DA_DCC_NONCONFORMING		equ 0x000		;Dc:�����ѡ����-��һ�´����

CONST_DA_EX_EXECUTABLE			equ 0x008		;Ex:��ִ�д����
CONST_DA_EX_DATA				equ 0x000		;Ex:����ѡ����

CONST_DA_S_CODE					equ 0x010		;S:�����
CONST_DA_S_DATA					equ 0x010		;S:���ݶ�
CONST_DA_S_GATE					equ	0x000		;S:��
CONST_DA_S_SYSTEM				equ 0x000		;S:ϵͳ��

CONST_DA_PRIV_R0				equ 0x000		;Privl: Ring0(�ں�)
CONST_DA_PRIV_R1				equ 0x020		;Privl: Ring1
CONST_DA_PRIV_R2				equ 0x040		;Privl: Ring2
CONST_DA_PRIV_R3				equ 0x060		;Privl: Ring3(�û�)

CONST_DA_PR_VALID				equ 0x080		;Pr:��Чѡ����
CONST_DA_PR_INVALID				equ 0x000		;Pr:��Чѡ����

CONST_DA_SZ_16					equ 0x000		;Sz: 16λ����ģʽ
CONST_DA_SZ_32					equ 0x400		;Sz: 32λ����ģʽ

CONST_DA_GR_1B					equ 0x000		;Gr: ������ 1B
CONST_DA_GR_4K					equ 0x800		;Gr: ������ 4B

CONST_DAPS_CGA_VIDEO_R3			equ  CONST_DA_RW_WRITABLE|CONST_DA_DCC_NONCONFORMING|CONST_DA_EX_DATA|CONST_DA_S_DATA|CONST_DA_PRIV_R3|CONST_DA_PR_VALID
CONST_DAPS_VGA_VIDEO_R3			equ  CONST_DA_RW_WRITABLE|CONST_DA_DCC_NONCONFORMING|CONST_DA_EX_DATA|CONST_DA_S_DATA|CONST_DA_PRIV_R3|CONST_DA_PR_VALID
CONST_DAPS_MDA_VIDEO_R3			equ  CONST_DA_RW_WRITABLE|CONST_DA_DCC_NONCONFORMING|CONST_DA_EX_DATA|CONST_DA_S_DATA|CONST_DA_PRIV_R3|CONST_DA_PR_VALID

CONST_DAPS_GENERAL_R0			equ  CONST_DA_RW_WRITABLE|CONST_DA_DCC_NONCONFORMING|CONST_DA_EX_EXECUTABLE|CONST_DA_S_CODE|CONST_DA_PRIV_R0|CONST_DA_PR_VALID|CONST_DA_GR_4K|CONST_DA_SZ_32
CONST_DAPS_GENERAL_DATA_R0		equ  CONST_DA_RW_WRITABLE|CONST_DA_DCC_NONCONFORMING|CONST_DA_EX_DATA|CONST_DA_S_DATA|CONST_DA_PRIV_R0|CONST_DA_PR_VALID|CONST_DA_GR_4K|CONST_DA_SZ_32

;�ѷ�������
CONST_DAOR_CR					equ 0x9A
CONST_DAOR_32					equ	4000h
CONST_DAOR_LIMIT_4K				equ	8000h

;----------------------------------			
; ��ѡ����(Segment Selector)
;----------------------------------

CONST_SA_RPL0					equ 0x0			;RPL: ������Ȩ�� 0
CONST_SA_RPL1					equ 0x1			;RPL: ������Ȩ�� 1
CONST_SA_RPL2					equ 0x2			;RPL: ������Ȩ�� 2
CONST_SA_RPL3					equ 0x3			;RPL: ������Ȩ�� 3

;----------------------------------			
; ��ʾ��Ϣ
;----------------------------------
CONST_LOADER_Loaded_b:
CONST_LOADER_Loaded						db ' Loader - Loaded'
CONST_LOADER_Loaded_LEN					equ $ - CONST_LOADER_Loaded_b

CONST_LOADER_GetMemory_Failure_b:
CONST_LOADER_GetMemory_Failure			db ' Get Memory - Failure'
CONST_LOADER_GetMemory_Failure_LEN		equ $ - CONST_LOADER_GetMemory_Failure_b

CONST_LOADER_GetMemory_Success_b:
CONST_LOADER_GetMemory_Success			db ' Get Memory - Done'
CONST_LOADER_GetMemory_Success_LEN		equ $ - CONST_LOADER_GetMemory_Success_b

CONST_LOADER_GetMemory_InProg_b:
CONST_LOADER_GetMemory_InProg			db ' Get Memory - In Progress'
CONST_LOADER_GetMemory_InProg_LEN		equ $ - CONST_LOADER_GetMemory_InProg_b

CONST_LOADER_GetMemory_IntroHeader		db 'BAL     ,BAH     ,LL      ,LH      ,Type'
CONST_LOADER_GetMemory_IntroHeader_LEN:	equ $ - CONST_LOADER_GetMemory_IntroHeader

CONST_LOADER_Disp_Space					db ' '
CONST_LOADER_Disp_Space_LEN				equ $ - CONST_LOADER_Disp_Space

CONST_LOADER_KrnlFile					db 'KERNEL  BIN',0
CONST_LOADER_KrnlFile_LEN				equ $ - CONST_LOADER_KrnlFile

CONST_LOADER_Loader_KrnlMissing			db ' Loader - Kernel Missing',0
CONST_LOADER_Loader_KrnlMissing_LEN		equ $ - CONST_LOADER_Loader_KrnlMissing

CONST_LOADER_Loader_KrnlLoaded			db ' Loader - Kernel Loaded',0
CONST_LOADER_Loader_KrnlLoaded_LEN		equ $ - CONST_LOADER_Loader_KrnlLoaded

CONST_LOADER_Loader_KrnlLoading			db ' Loader - Now Loading Kernel',0
CONST_LOADER_Loader_KrnlLoading_LEN		equ $ - CONST_LOADER_Loader_KrnlLoading


;----------------------------------			
; ����ģʽ��ʾ��Ϣ (�ڴ����ӳ���ע��\0��β)
;----------------------------------
CONST_LOADERPM_Loader_PMEntered			db ' Loader - Running In Protected Mode',0
CONSTP_LOADERPM_Loader_PMEntered		equ CONST_LOADER_BasePhyAddr + CONST_LOADERPM_Loader_PMEntered

;----------------------------------			
; �Դ����ʾ
;----------------------------------
CONST_VIDEOMEMORY_CGA_COLTEXT_BASE		equ 0xb8000
CONST_VIDEOMEMORY_CGA_COLTEXT_END		equ 0xbffff
CONST_VIDEOMEMORY_CGA_LIMIT				equ 0x0ffff
CONST_VIDEOMEMORY_VGA_BASE				equ 0xa0000
CONST_VIDEOMEMORY_VGA_END				equ 0xaffff
CONST_VIDEOMEMORY_VGA_LIMIT				equ 0x0ffff
CONST_VIDEOMEMORY_MDA_BASE				equ 0xb0000
CONST_VIDEOMEMORY_MDA_END				equ 0xb7fff
CONST_VIDEOMEMORY_MDA_LIMIT				equ 0x07fff

CONST_VIDEO_CGA_COLS					equ 80

;----------------------------------			
; �ڴ��ҳ
;----------------------------------
CONST_MEMORY_PageDirBase		equ	200000h	
CONST_MEMORY_PageTblBase		equ	201000h	

CONST_MEMORYPG_P				EQU	1	; ҳ��������λ
CONST_MEMORYPG_RWR				EQU	0	; R/W ����λֵ, ��/ִ��
CONST_MEMORYPG_RWW				EQU	2	; R/W ����λֵ, ��/д/ִ��
CONST_MEMORYPG_USS				EQU	0	; U/S ����λֵ, ϵͳ��
CONST_MEMORYPG_USU				EQU	4	; U/S ����λֵ, �û���

;----------------------------------			
; Kernel ���ص�ַ
;----------------------------------
CONST_KERNEL_BaseAddr				equ 0x7000				;Kernel ���ضε�ַ
CONST_KERNEL_OffsetAddr				equ 0x0					;Kernel ���ص�ַƫ��
CONST_KERNEL_BasePhyAddr			equ CONST_KERNEL_BaseAddr * 0x10 ;Kernel ���ضε�ַ������ַ	

CONST_KERNEL_Entry					equ 0x30400				;Kernel ��ڵ�ַ

;----------------------------------			
; ����
;----------------------------------
;�ݴ�Ĵ���ֵ
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