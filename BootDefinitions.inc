; ---------------------------------------------------------------			
; BootDefinitions.inc
; ---------------------------------------------------------------	
; Boot Definitons
; 引导程序常量 (引导扇区定义)
; By Aeroraven, 2021-3-12
; --------------------------------------------------------------
; 参考: Forrest Yu
; ---------------------------------------------------------------

;----------------------------------			
; 引导扇区
;----------------------------------
CONST_FAT12_BS_OEMName				db 'AERORVEN'			;厂商名, 8 Bytes
CONST_FAT12_BPB_BytsPerSec			dw 0x200				;扇区字节, 2 Bytes
CONST_FAT12_BPB_SecPerClus			db 0x1					;每簇扇区数, 1 Byte
CONST_FAT12_BPB_RsvdSecCnt			dw 0x1					;保留扇区数, 2 Byte
CONST_FAT12_BPB_NumFATs				db 0x2					;FAT表数, 1 Byte
CONST_FAT12_BPB_RootEntCnt			dw 0xE0					;根条目数, 2 Bytes
CONST_FAT12_BPB_TotSec16			dw 0xB40				;总扇区数, 2 Bytes
CONST_FAT12_BPB_Media				db 0xF0					;媒介描述, 1 Byte
CONST_FAT12_BPB_FATSz16				dw 0x9					;FAT大小, 2 Bytes
CONST_FAT12_BPB_SeccPerTrk			dw 0x12					;每磁道扇区数, 2 Bytes
CONST_FAT12_BPB_NumHeads			dw 0x2					;磁头数, 2 Bytes
CONST_FAT12_BPB_HiddSec				dd 0x0					;隐藏扇区数, 2 Bytes
CONST_FAT12_BPB_TotSec32			dd 0x0					;总扇区数32, 4 Bytes
CONST_FAT12_BS_DrvNum				db 0x0					;硬盘标识, 1 Byte
CONST_FAT12_BS_Reserved1			db 0x0					;
CONST_FAT12_BS_BootSig				db 0x29					;启动标识, 1 Byte
CONST_FAT12_BS_VolID				dd 0x0					;卷ID,4 Bytes
CONST_FAT12_BS_VolLab				db 'AeroravenFL'		;卷标,11 Bytes
CONST_FAT12_BS_FileSysType			db 'FAT12   '			;文件系统类型, 8 Bytes
CONST_FAT12_BS_EndSign				equ 0xAA55				;扇区结束标识, 2 Bytes
CONST_FAT12_BS_SectorNoFAT1			equ CONST_FAT12_BPB_RsvdSecCnt	;FAT1扇区号

;----------------------------------			
; 调试宏(用于Bochs断点)
;----------------------------------
%macro BREAK_PT 0
	xchg bx,bx
%endmacro;