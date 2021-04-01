; ---------------------------------------------------------------			
; BootDefinitions.inc
; ---------------------------------------------------------------	
; Boot Definitons
; ���������� (������������)
; By Aeroraven, 2021-3-12
; --------------------------------------------------------------
; �ο�: Forrest Yu
; ---------------------------------------------------------------

;----------------------------------			
; ��������
;----------------------------------
CONST_FAT12_BS_OEMName				db 'AERORVEN'			;������, 8 Bytes
CONST_FAT12_BPB_BytsPerSec			dw 0x200				;�����ֽ�, 2 Bytes
CONST_FAT12_BPB_SecPerClus			db 0x1					;ÿ��������, 1 Byte
CONST_FAT12_BPB_RsvdSecCnt			dw 0x1					;����������, 2 Byte
CONST_FAT12_BPB_NumFATs				db 0x2					;FAT����, 1 Byte
CONST_FAT12_BPB_RootEntCnt			dw 0xE0					;����Ŀ��, 2 Bytes
CONST_FAT12_BPB_TotSec16			dw 0xB40				;��������, 2 Bytes
CONST_FAT12_BPB_Media				db 0xF0					;ý������, 1 Byte
CONST_FAT12_BPB_FATSz16				dw 0x9					;FAT��С, 2 Bytes
CONST_FAT12_BPB_SeccPerTrk			dw 0x12					;ÿ�ŵ�������, 2 Bytes
CONST_FAT12_BPB_NumHeads			dw 0x2					;��ͷ��, 2 Bytes
CONST_FAT12_BPB_HiddSec				dd 0x0					;����������, 2 Bytes
CONST_FAT12_BPB_TotSec32			dd 0x0					;��������32, 4 Bytes
CONST_FAT12_BS_DrvNum				db 0x0					;Ӳ�̱�ʶ, 1 Byte
CONST_FAT12_BS_Reserved1			db 0x0					;
CONST_FAT12_BS_BootSig				db 0x29					;������ʶ, 1 Byte
CONST_FAT12_BS_VolID				dd 0x0					;��ID,4 Bytes
CONST_FAT12_BS_VolLab				db 'AeroravenFL'		;����,11 Bytes
CONST_FAT12_BS_FileSysType			db 'FAT12   '			;�ļ�ϵͳ����, 8 Bytes
CONST_FAT12_BS_EndSign				equ 0xAA55				;����������ʶ, 2 Bytes
CONST_FAT12_BS_SectorNoFAT1			equ CONST_FAT12_BPB_RsvdSecCnt	;FAT1������

;----------------------------------			
; ���Ժ�(����Bochs�ϵ�)
;----------------------------------
%macro BREAK_PT 0
	xchg bx,bx
%endmacro;