; ---------------------------------------------------------------			
; BootDefinitionExt.inc
; ---------------------------------------------------------------	
; Boot Definitons
; 引导程序值定义
; By Aeroraven, 2021-3-12
; --------------------------------------------------------------


;----------------------------------			
; FAT12相关常量(在Boot.asm)中初始化
;----------------------------------

CONST_FAT12_BS_RootDirSector		db 0x0					;根目录扇区数 
;CONST_FAT12_BS_SecNoOfRootDir		db 0x0					;根目录起始扇区(节省空间，直接赋值dwSector）

CONST_FAT12_BS_DeltaSectorNo		db 0x0					;数据区起始扇区

;----------------------------------
; 启动常量
;----------------------------------
CONST_BOOT_Msg0b:
CONST_BOOT_Msg0						db ' Booting'
CONST_BOOT_Msg0_Len					equ $ - CONST_BOOT_Msg0b

CONST_BOOT_Msg1b:
CONST_BOOT_Msg1						db ' NoLoader'
CONST_BOOT_Msg1_Len					equ $ - CONST_BOOT_Msg1b

CONST_BOOT_Msg2b:
CONST_BOOT_Msg2						db ' Done'
CONST_BOOT_Msg2_Len					equ $ - CONST_BOOT_Msg2b

CONST_BOOT_LoaderName				db 'LOADER  BIN',0

;----------------------------------
; Loader常量
;----------------------------------
CONST_LOADER_BaseAddr				equ 0x9000				;Loader 加载段地址
CONST_LOADER_OffsetAddr				equ 0x100				;Loader 加载地址偏移
CONST_LOADER_BasePhyAddr			equ CONST_LOADER_BaseAddr * 0x10 ;Loader 加载段地址物理地址	