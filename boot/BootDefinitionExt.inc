; ---------------------------------------------------------------			
; BootDefinitionExt.inc
; ---------------------------------------------------------------	
; Boot Definitons
; ��������ֵ����
; By Aeroraven, 2021-3-12
; --------------------------------------------------------------


;----------------------------------			
; FAT12��س���(��Boot.asm)�г�ʼ��
;----------------------------------

CONST_FAT12_BS_RootDirSector		db 0x0					;��Ŀ¼������ 
;CONST_FAT12_BS_SecNoOfRootDir		db 0x0					;��Ŀ¼��ʼ����(��ʡ�ռ䣬ֱ�Ӹ�ֵdwSector��

CONST_FAT12_BS_DeltaSectorNo		db 0x0					;��������ʼ����

;----------------------------------
; ��������
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
; Loader����
;----------------------------------
CONST_LOADER_BaseAddr				equ 0x9000				;Loader ���ضε�ַ
CONST_LOADER_OffsetAddr				equ 0x100				;Loader ���ص�ַƫ��
CONST_LOADER_BasePhyAddr			equ CONST_LOADER_BaseAddr * 0x10 ;Loader ���ضε�ַ������ַ	