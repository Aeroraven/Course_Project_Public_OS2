; ---------------------------------------------------------------			
; LoaderDefinitions.asm
; ---------------------------------------------------------------	
; Boot Loader Definitions
; 引导程序常量和定义
; By Aeroraven, 2021-3-13
; --------------------------------------------------------------
; 参考: Forrest Yu
; --------------------------------------------------------------


;----------------------------------			
; 描述符(Descriptor)
;----------------------------------
%macro DESCRIPTOR 3 ;DESCRIPTOR Base,Limit,Attr
	dw (%2 & 0FFFFh)							;界限0:15			(0-15位)
	dw (%1 & 0FFFFh)							;基址0:15			(16-31位)
	db (%1 >> 16) & 0FFh						;基址16:24			(32-39位)
	db (%3 & 0FFFFh)							;访问字节			(40-47位)
	db ((%2 >> 16) & 00Fh )| ((%3 >> 4) & 0F0h)	;界限16:19 + 标识	(48-55位)
	db (%1 >> 24) & 0FFh						;基址24:31			(56-63位）
%endmacro

;访问字节结构: Ac,Rw,Dc,Ex,S,Privl,Present
;标识结构: 0,0,Sz,Gr

CONST_DA_AC_ACCESSED			equ 0x001		;Ac:段被访问
CONST_DA_AC_DEFAULT				equ 0x000		;Ac:默认值

CONST_DA_RW_READONLY			equ 0x000		;Rw:段只读
CONST_DA_RW_WRITABLE			equ 0x002		;Rw:段可读写

CONST_DA_DCD_UP					equ 0x000		;Dc:数据选择子-段方向向上
CONST_DA_DCD_DOWN				equ 0x004		;Dc:数据选择子-段方向向下
CONST_DA_DCC_CONFORMING			equ 0x004		;Dc:代码段选择子-一致代码段
CONST_DA_DCC_NONCONFORMING		equ 0x000		;Dc:代码段选择子-非一致代码段

CONST_DA_EX_EXECUTABLE			equ 0x008		;Ex:可执行代码段
CONST_DA_EX_DATA				equ 0x000		;Ex:数据选择子

CONST_DA_S_CODE					equ 0x010		;S:代码段
CONST_DA_S_DATA					equ 0x010		;S:数据段
CONST_DA_S_GATE					equ	0x000		;S:门
CONST_DA_S_SYSTEM				equ 0x000		;S:系统段

CONST_DA_PRIV_R0				equ 0x000		;Privl: Ring0(内核)
CONST_DA_PRIV_R1				equ 0x020		;Privl: Ring1
CONST_DA_PRIV_R2				equ 0x040		;Privl: Ring2
CONST_DA_PRIV_R3				equ 0x060		;Privl: Ring3(用户)

CONST_DA_PR_VALID				equ 0x080		;Pr:有效选择子
CONST_DA_PR_INVALID				equ 0x000		;Pr:无效选择子

CONST_DA_SZ_16					equ 0x000		;Sz: 16位保护模式
CONST_DA_SZ_32					equ 0x400		;Sz: 32位保护模式

CONST_DA_GR_1B					equ 0x000		;Gr: 段粒度 1B
CONST_DA_GR_4K					equ 0x800		;Gr: 段粒度 4B

CONST_DAPS_CGA_VIDEO_R3			equ  CONST_DA_RW_WRITABLE|CONST_DA_DCC_NONCONFORMING|CONST_DA_EX_DATA|CONST_DA_S_DATA|CONST_DA_PRIV_R3|CONST_DA_PR_VALID
CONST_DAPS_VGA_VIDEO_R3			equ  CONST_DA_RW_WRITABLE|CONST_DA_DCC_NONCONFORMING|CONST_DA_EX_DATA|CONST_DA_S_DATA|CONST_DA_PRIV_R3|CONST_DA_PR_VALID
CONST_DAPS_MDA_VIDEO_R3			equ  CONST_DA_RW_WRITABLE|CONST_DA_DCC_NONCONFORMING|CONST_DA_EX_DATA|CONST_DA_S_DATA|CONST_DA_PRIV_R3|CONST_DA_PR_VALID

CONST_DAPS_GENERAL_R0			equ  CONST_DA_RW_WRITABLE|CONST_DA_DCC_NONCONFORMING|CONST_DA_EX_EXECUTABLE|CONST_DA_S_CODE|CONST_DA_PRIV_R0|CONST_DA_PR_VALID|CONST_DA_GR_4K|CONST_DA_SZ_32
CONST_DAPS_GENERAL_DATA_R0		equ  CONST_DA_RW_WRITABLE|CONST_DA_DCC_NONCONFORMING|CONST_DA_EX_DATA|CONST_DA_S_DATA|CONST_DA_PRIV_R0|CONST_DA_PR_VALID|CONST_DA_GR_4K|CONST_DA_SZ_32

;已废弃常量
CONST_DAOR_CR					equ 0x9A
CONST_DAOR_32					equ	4000h
CONST_DAOR_LIMIT_4K				equ	8000h

;----------------------------------			
; 段选择子(Segment Selector)
;----------------------------------

CONST_SA_RPL0					equ 0x0			;RPL: 请求特权级 0
CONST_SA_RPL1					equ 0x1			;RPL: 请求特权级 1
CONST_SA_RPL2					equ 0x2			;RPL: 请求特权级 2
CONST_SA_RPL3					equ 0x3			;RPL: 请求特权级 3

;----------------------------------			
; 提示信息
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
; 保护模式提示信息 (在此添加常量注意\0结尾)
;----------------------------------
CONST_LOADERPM_Loader_PMEntered			db ' Loader - Running In Protected Mode',0
CONSTP_LOADERPM_Loader_PMEntered		equ CONST_LOADER_BasePhyAddr + CONST_LOADERPM_Loader_PMEntered

;----------------------------------			
; 显存和显示
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
; 内存分页
;----------------------------------
CONST_MEMORY_PageDirBase		equ	200000h	
CONST_MEMORY_PageTblBase		equ	201000h	

CONST_MEMORYPG_P				EQU	1	; 页存在属性位
CONST_MEMORYPG_RWR				EQU	0	; R/W 属性位值, 读/执行
CONST_MEMORYPG_RWW				EQU	2	; R/W 属性位值, 读/写/执行
CONST_MEMORYPG_USS				EQU	0	; U/S 属性位值, 系统级
CONST_MEMORYPG_USU				EQU	4	; U/S 属性位值, 用户级

;----------------------------------			
; Kernel 加载地址
;----------------------------------
CONST_KERNEL_BaseAddr				equ 0x7000				;Kernel 加载段地址
CONST_KERNEL_OffsetAddr				equ 0x0					;Kernel 加载地址偏移
CONST_KERNEL_BasePhyAddr			equ CONST_KERNEL_BaseAddr * 0x10 ;Kernel 加载段地址物理地址	

CONST_KERNEL_Entry					equ 0x30400				;Kernel 入口地址

;----------------------------------			
; 杂项
;----------------------------------
;暂存寄存器值
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
