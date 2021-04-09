; ---------------------------------------------------------------			
; Boot.asm
; ---------------------------------------------------------------	
; Boot Program
; 引导程序
; By Aeroraven, 2021-3-12
; --------------------------------------------------------------

	org 07c00h
	jmp  LABEL_BOOT_DEFINITION_INIT
	nop			
	%include "BootDefinitions.inc"
	%include "BIOSInterruptList.inc"
	
LABEL_BOOT_DEFINITION_INIT:

	;初始化段寄存器(ds,es,ss)和栈指针(sp)
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov sp, 0x7c00

	;初始化 CONST_FAT12_BS_RootDirSector(dwRootDirSize)
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

	;初始化 CONST_FAT12_BS_SecNoOfRootDir(dwSector)
	mov al, [CONST_FAT12_BPB_NumFATs]
	mov bx, [CONST_FAT12_BPB_FATSz16]
	mul bx
	add ax, word [CONST_FAT12_BPB_RsvdSecCnt]
	mov [dwSector], ax

	;初始化 CONST_FAT12_BS_DeltaSectorNo
	sub ax, 2
	mov [CONST_FAT12_BS_DeltaSectorNo], ax

	

	;重置软盘
	INT13_RstDskSys [CONST_FAT12_BS_DrvNum]

	
	;显示提示字符串
	INT10_ScrollUp 0, CONST_BINT_WHITE_FG|CONST_BINT_BLACK_BG, 0x0, 0xffff
	mov cx, CONST_BOOT_Msg0_Len
	mov bp, CONST_BOOT_Msg0
	call FUNC_DisplayString

LABEL_SEARCHING_LOADER:
	;判断根目录所有扇区是否均被找完
	cmp word [dwRootDirSize],0
	dec word [dwRootDirSize]
	jz LABEL_LOADER_MISSING
	;设输出地址(ES:BX) [INT13输出]
	mov ax, CONST_LOADER_BaseAddr
	mov es, ax
	mov bx, CONST_LOADER_OffsetAddr
	mov ax,[dwSector]
	mov ch,1
	
	call FUNC_ReadSector
	
	;ES:DI为偏移地址
	mov di,CONST_LOADER_OffsetAddr
	cld
	;一个扇区有512字节(根据BootDefinitions.inc)，一个条目占用32字节,故有16条目
	mov dx,10h
LABEL_COMPARE_ENTRY:

	
	;设DS:SI为串地址
	mov si,CONST_BOOT_LoaderName
	cmp dx,0
	jz LABEL_NEXT_SECTOR
	dec dx
	mov cx,0xb
LABEL_COMPARE_FILENAME:
	;比较文件名
	jcxz LABEL_FILE_FOUND
	dec cx
	lodsb
	cmp al,byte[es:di]
	;继续匹配下一字符
	jz LABEL_NEXT_CHAR
	;没找到 - 消除 inc di的影响(+11次di最多影响11位)
	and di,0ffe0h
	;di指向下一条目
	add di,20h
	jmp LABEL_COMPARE_ENTRY

LABEL_NEXT_CHAR:
	;下一字符
	inc di
	jmp LABEL_COMPARE_FILENAME
	
LABEL_FILE_FOUND:

	;DI指向开始簇号
	and di, 0ffe0h
	add di, 0x1A
	;AX 指向文件在FAT表中的起始条目对应的编号
	mov ax,word[es:di]
	push ax
	;AX 指向文件起始扇区号
	add al, byte [CONST_FAT12_BS_RootDirSector]
	add al, byte [CONST_FAT12_BS_DeltaSectorNo]

	
	;ES:BX 指向加载地址
	mov cx,CONST_LOADER_BaseAddr
	mov es,cx
	mov bx,CONST_LOADER_OffsetAddr

LABEL_LOAD_READ:
	;读取第一个扇区
	
	mov cx, 0x0100
	call FUNC_ReadSector
	pop ax
	
	call FUNC_GetFATEntry
	;如果FAT指向下一编号位0xFFF则说明已到达文件尾
	cmp ax,0x0FFF
	jz LABEL_FILE_LOADED

	push ax
	add al,  [CONST_FAT12_BS_RootDirSector]
	add al, [CONST_FAT12_BS_DeltaSectorNo]
	add bx, [CONST_FAT12_BPB_BytsPerSec]
	jmp LABEL_LOAD_READ


LABEL_NEXT_SECTOR:
	;下一扇区
	inc word [dwSector]
	jmp LABEL_SEARCHING_LOADER

LABEL_FILE_LOADED:
	;完成加载
	mov cx, CONST_BOOT_Msg2_Len
	mov bp, CONST_BOOT_Msg2
	call FUNC_DisplayString
	;BREAK_PT
	jmp CONST_LOADER_BaseAddr:CONST_LOADER_OffsetAddr

LABEL_LOADER_MISSING:
	;根目录所有扇区均被找完，没有发现Loader
	mov cx, CONST_BOOT_Msg1_Len
	mov bp, CONST_BOOT_Msg1
	call FUNC_DisplayString
	
	jmp $

 
;函数 DisplayString
;参数:	cx 字符串长度(预设在BootDefinitionExt.inc)
;		bp 字符串偏移地址(预设在BootDefinitionExt.inc)	
FUNC_DisplayString:
	mov ax,ds
	mov es,ax
	xor dx,dx
	mov dh,[dbShownLine]
	INT10_DispStr 0,1,CONST_BINT_WHITE_FG|CONST_BINT_BLACK_BG
	inc dh
	mov [dbShownLine],dh
	ret

;函数 ReadSector
;参数:	es:bx 接收读取内容的地址(由INT 13H决定)
;		ch 读取扇区数
;		ax 起始扇区号
FUNC_ReadSector:
	;保存寄存器
	push bp
	mov bp, sp
	sub	esp, 2			
	mov	byte [bp-2], ch
	push bx
	mov bl, [CONST_FAT12_BPB_SeccPerTrk]
	div bl  ;商->AL;余数->AH
	pop bx
	;CH=柱面号
	mov ch,al
	shr ch,1
	;DH=磁头号
	mov dh,al 
	and dh,1
	;CL=起始扇区号
	inc ah
	mov cl,ah
	;DL=硬盘号
	xor dl,dl
	INT13_RdSector3 byte[bp-2]
	;恢复栈
	add esp,2
	pop bp
	ret

;函数 GetFATEntry
;参数	ax FAT编号
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
	add	ax, 1 ;根据BootDefinitions.inc设定
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


;下面是数据部分
VARIABLES:
	%include "BootDefinitionExt.inc"

	dwSector		dw 0		;当前查找扇区号
	dwRootDirSize	dw 0		;根目录剩余未遍历扇区数
	dbOddFlag		db 0
	dbShownLine		db 1


;填充空白
times 	510-($-$$)	db	0	
dw 	CONST_FAT12_BS_EndSign				