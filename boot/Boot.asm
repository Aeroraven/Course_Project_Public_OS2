; ---------------------------------------------------------------			
; Boot.asm
; ---------------------------------------------------------------	
; Boot Program
; ��������
; By Aeroraven, 2021-3-12
; --------------------------------------------------------------

	org 07c00h
	jmp  LABEL_BOOT_DEFINITION_INIT
	nop			
	%include "BootDefinitions.inc"
	%include "BIOSInterruptList.inc"
	
LABEL_BOOT_DEFINITION_INIT:

	;��ʼ���μĴ���(ds,es,ss)��ջָ��(sp)
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov sp, 0x7c00

	;��ʼ�� CONST_FAT12_BS_RootDirSector(dwRootDirSize)
	mov ax, [CONST_FAT12_BPB_RootEntCnt]
	mov bx, 0x0020
	mul bx
	add ax, [CONST_FAT12_BPB_BytsPerSec]
	dec ax
	mov bx, [CONST_FAT12_BPB_BytsPerSec]
	div bx
	xor ah,ah
	mov [dwRootDirSize], ax
	mov [CONST_FAT12_BS_RootDirSector], ax

	;��ʼ�� CONST_FAT12_BS_SecNoOfRootDir(dwSector)
	mov al, [CONST_FAT12_BPB_NumFATs]
	mov bx, [CONST_FAT12_BPB_FATSz16]
	mul bx
	add ax, word [CONST_FAT12_BPB_RsvdSecCnt]
	mov [dwSector], ax

	;��ʼ�� CONST_FAT12_BS_DeltaSectorNo
	sub ax, 2
	mov [CONST_FAT12_BS_DeltaSectorNo], ax

	

	;��������
	INT13_RstDskSys [CONST_FAT12_BS_DrvNum]

	
	;��ʾ��ʾ�ַ���
	INT10_ScrollUp 0, CONST_BINT_WHITE_FG|CONST_BINT_BLACK_BG, 0x0, 0xffff
	mov cx, CONST_BOOT_Msg0_Len
	mov bp, CONST_BOOT_Msg0
	call FUNC_DisplayString

LABEL_SEARCHING_LOADER:
	;�жϸ�Ŀ¼���������Ƿ��������
	cmp word [dwRootDirSize],0
	dec word [dwRootDirSize]
	jz LABEL_LOADER_MISSING
	;�������ַ(ES:BX) [INT13���]
	mov ax, CONST_LOADER_BaseAddr
	mov es, ax
	mov bx, CONST_LOADER_OffsetAddr
	mov ax,[dwSector]
	mov ch,1
	
	call FUNC_ReadSector
	
	;ES:DIΪƫ�Ƶ�ַ
	mov di,CONST_LOADER_OffsetAddr
	cld
	;һ��������512�ֽ�(����BootDefinitions.inc)��һ����Ŀռ��32�ֽ�,����16��Ŀ
	mov dx,10h
LABEL_COMPARE_ENTRY:

	
	;��DS:SIΪ����ַ
	mov si,CONST_BOOT_LoaderName
	cmp dx,0
	jz LABEL_NEXT_SECTOR
	dec dx
	mov cx,0xb
LABEL_COMPARE_FILENAME:
	;�Ƚ��ļ���
	jcxz LABEL_FILE_FOUND
	dec cx
	lodsb
	cmp al,byte[es:di]
	;����ƥ����һ�ַ�
	jz LABEL_NEXT_CHAR
	;û�ҵ� - ���� inc di��Ӱ��(+11��di���Ӱ��11λ)
	and di,0ffe0h
	;diָ����һ��Ŀ
	add di,20h
	jmp LABEL_COMPARE_ENTRY

LABEL_NEXT_CHAR:
	;��һ�ַ�
	inc di
	jmp LABEL_COMPARE_FILENAME
	
LABEL_FILE_FOUND:

	;DIָ��ʼ�غ�
	and di, 0ffe0h
	add di, 0x1A
	;AX ָ���ļ���FAT���е���ʼ��Ŀ��Ӧ�ı��
	mov ax,word[es:di]
	push ax
	;AX ָ���ļ���ʼ������
	add al, byte [CONST_FAT12_BS_RootDirSector]
	add al, byte [CONST_FAT12_BS_DeltaSectorNo]

	
	;ES:BX ָ����ص�ַ
	mov cx,CONST_LOADER_BaseAddr
	mov es,cx
	mov bx,CONST_LOADER_OffsetAddr

LABEL_LOAD_READ:
	;��ȡ��һ������
	
	mov cx, 0x0100
	call FUNC_ReadSector
	pop ax
	
	call FUNC_GetFATEntry
	;���FATָ����һ���λ0xFFF��˵���ѵ����ļ�β
	cmp ax,0x0FFF
	jz LABEL_FILE_LOADED

	push ax
	add al,  [CONST_FAT12_BS_RootDirSector]
	add al, [CONST_FAT12_BS_DeltaSectorNo]
	add bx, [CONST_FAT12_BPB_BytsPerSec]
	jmp LABEL_LOAD_READ


LABEL_NEXT_SECTOR:
	;��һ����
	inc word [dwSector]
	jmp LABEL_SEARCHING_LOADER

LABEL_FILE_LOADED:
	;��ɼ���
	mov cx, CONST_BOOT_Msg2_Len
	mov bp, CONST_BOOT_Msg2
	call FUNC_DisplayString
	;BREAK_PT
	jmp CONST_LOADER_BaseAddr:CONST_LOADER_OffsetAddr

LABEL_LOADER_MISSING:
	;��Ŀ¼���������������꣬û�з���Loader
	mov cx, CONST_BOOT_Msg1_Len
	mov bp, CONST_BOOT_Msg1
	call FUNC_DisplayString
	
	jmp $

 
;���� DisplayString
;����:	cx �ַ�������(Ԥ����BootDefinitionExt.inc)
;		bp �ַ���ƫ�Ƶ�ַ(Ԥ����BootDefinitionExt.inc)	
FUNC_DisplayString:
	mov ax,ds
	mov es,ax
	xor dx,dx
	mov dh,[dbShownLine]
	INT10_DispStr 0,1,CONST_BINT_WHITE_FG|CONST_BINT_BLACK_BG
	inc dh
	mov [dbShownLine],dh
	ret

;���� ReadSector
;����:	es:bx ���ն�ȡ���ݵĵ�ַ(��INT 13H����)
;		ch ��ȡ������
;		ax ��ʼ������
FUNC_ReadSector:
	;����Ĵ���
	push bp
	mov bp, sp
	sub	esp, 2			
	mov	byte [bp-2], ch
	push bx
	mov bl, [CONST_FAT12_BPB_SeccPerTrk]
	div bl  ;��->AL;����->AH
	pop bx
	;CH=�����
	mov ch,al
	shr ch,1
	;DH=��ͷ��
	mov dh,al 
	and dh,1
	;CL=��ʼ������
	inc ah
	mov cl,ah
	;DL=Ӳ�̺�
	xor dl,dl
	INT13_RdSector3 byte[bp-2]
	;�ָ�ջ
	add esp,2
	pop bp
	ret

;���� GetFATEntry
;����	ax FAT���
;REFERENCE: Forrest Yu
FUNC_GetFATEntry:
	push es
	push bx
	mov bx, CONST_LOADER_BaseAddr 
	sub bx, 0100h  
	mov es, bx   
	mov byte [dbOddFlag], 0
	mov bx, 3
	mul bx   
	mov bx, 2
	div bx   
	cmp dx, 0
	jz LABEL_EVEN
	mov byte [dbOddFlag], 1
LABEL_EVEN:
	xor dx, dx   
	mov bx, [CONST_FAT12_BPB_BytsPerSec]
	div bx   
	push dx
	xor bx,bx  
	add	ax, 1 ;����BootDefinitions.inc�趨
	mov cx, 0x0200
	call FUNC_ReadSector  
	pop dx
	add bx, dx
	mov ax, [es:bx]
	cmp byte [dbOddFlag], 1
	jnz LABEL_EVEN_2
	shr ax, 4
LABEL_EVEN_2:
	and ax, 0FFFh

LABEL_GET_FAT_ENRY_OK:
	 pop bx
	 pop es
	 ret


;���������ݲ���
VARIABLES:
	%include "BootDefinitionExt.inc"

	dwSector		dw 0		;��ǰ����������
	dwRootDirSize	dw 0		;��Ŀ¼ʣ��δ����������
	dbOddFlag		db 0
	dbShownLine		db 1


;���հ�
times 	510-($-$$)	db	0	
dw 	CONST_FAT12_BS_EndSign				