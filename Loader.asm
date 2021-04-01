; ---------------------------------------------------------------			
; Loader.asm
; ---------------------------------------------------------------	
; Boot Loader
; 引导程序 Loader
; By Aeroraven, 2021-3-13
; --------------------------------------------------------------
org  0100h
	jmp LABEL_PROGRAM_START
	
	%include "BIOSInterruptList.inc"
	%include "BootDefinitions.inc"
	%include "BootDefinitionExt.inc"
	%include "LoaderDefinitions.inc"

VARIABLES_REAL_MODE_1:
	DB_BINT_VIDEO_ACTIVE_PAGE		db 0x0
	DW_I							dw 0x0
	DD_MEMORY_CONTINUATION			dd 0x0
	DB_DISPLAY_PRINTLINE			db 0x1
	DB_DISPLAY_PRINTCOL				db 0x0
	DB_DISPLAY_INTLEN				db 0x0
	DB_DISPLAY_INTTMP				db 0x0
	DB_DISPLAY_CURROW				db 0x0
	DB_DISPLAY_CURCOL				db 0x0
	DBF_MEMORY_ARDS_BUFFER:			times 256	db 0

	DW_LOADER_LOADING_SECTOR		dw 0x0
	DW_LOADER_ROOTDIR_SIZE			dw 0x0
	DB_LOADER_ODD_FLAG				db 0x0

	DD_TEST							dd 0x19260817
	DS_TEST							db 'HelloWorld',0

	DD_MEMORY_SIZE					dd 0x0

	DW_TMP							dw 0x0

VARIABLES_PROTECTED_MODE_1:
	DBP_DISPLAY_PRINTLINE			equ DB_DISPLAY_PRINTLINE + CONST_LOADER_BasePhyAddr
	DBP_DISPLAY_PRINTCOL			equ DB_DISPLAY_PRINTCOL + CONST_LOADER_BasePhyAddr

	DDP_TEST						equ DD_TEST + CONST_LOADER_BasePhyAddr
	DSP_TEST						equ DS_TEST + CONST_LOADER_BasePhyAddr

	EQP_STACKTOP					equ 0x100 + CONST_LOADER_BasePhyAddr
	DDP_MEMORY_SIZE					equ DD_MEMORY_SIZE + CONST_LOADER_BasePhyAddr
	

LABEL_GDT:
	DESC_EMPTY:						DESCRIPTOR 0, 0, 0
	DESC_GENERAL:					DESCRIPTOR 0, 0fffffh, CONST_DAPS_GENERAL_R0
	DESC_GENERAL_DATA:				DESCRIPTOR 0, 0fffffh, CONST_DAPS_GENERAL_DATA_R0
	DESC_CGA_VIDEO:					DESCRIPTOR CONST_VIDEOMEMORY_CGA_COLTEXT_BASE, CONST_VIDEOMEMORY_CGA_LIMIT, CONST_DAPS_CGA_VIDEO_R3
	DESC_VGA_VIDEO:					DESCRIPTOR CONST_VIDEOMEMORY_VGA_BASE, CONST_VIDEOMEMORY_VGA_LIMIT, CONST_DAPS_VGA_VIDEO_R3
	DESC_MDA_VIDEO:					DESCRIPTOR CONST_VIDEOMEMORY_MDA_BASE, CONST_VIDEOMEMORY_MDA_LIMIT, CONST_DAPS_MDA_VIDEO_R3

LABEL_GDTPTR:
	EQ_GDTLEN						equ $-LABEL_GDT
	PTR_GDTPTR						dw EQ_GDTLEN-1
									dd CONST_LOADER_BasePhyAddr + LABEL_GDT

LABEL_SELECTOR:
	SELECTOR_EMPTY					equ DESC_EMPTY - LABEL_GDT 
	SELECTOR_GENERAL				equ DESC_GENERAL - LABEL_GDT 
	SELECTOR_GENERAL_DATA			equ DESC_GENERAL_DATA - LABEL_GDT
	SELECTOR_CGA_VIDEO				equ DESC_CGA_VIDEO - LABEL_GDT + CONST_SA_RPL3
	SELECTOR_VGA_VIDEO				equ DESC_VGA_VIDEO - LABEL_GDT + CONST_SA_RPL3
	SELECTOR_MDA_VIDEO				equ DESC_MDA_VIDEO - LABEL_GDT + CONST_SA_RPL3
	
LABEL_PROGRAM_START:
	;初始化段寄存器和栈
	mov ax,cs
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax ;!!!!! 不加报#BR异常 TAT
	mov sp,0100h

	INT10_ScrollUp 0, CONST_BINT_WHITE_FG|CONST_BINT_BLACK_BG, 0x0, 0xffff
	mov cx,CONST_LOADER_Loaded_LEN
	mov bp,CONST_LOADER_Loaded
	call _FUNC_DisplayString

	
	;FAT读取相关量初始化
	mov ax, [CONST_FAT12_BPB_RootEntCnt]
	mov bx, 0x0020
	mul bx
	add ax, [CONST_FAT12_BPB_BytsPerSec]
	dec ax
	mov bx, [CONST_FAT12_BPB_BytsPerSec]
	div bx
	xor ah,ah
	mov [DW_LOADER_ROOTDIR_SIZE], ax
	mov [CONST_FAT12_BS_RootDirSector], ax
	mov al, [CONST_FAT12_BPB_NumFATs]
	mov bx, [CONST_FAT12_BPB_FATSz16]
	mul bx
	add ax, word [CONST_FAT12_BPB_RsvdSecCnt]
	mov [DW_LOADER_LOADING_SECTOR], ax
	sub ax, 2
	mov [CONST_FAT12_BS_DeltaSectorNo], ax
	
	call _FUNC_DisplayNewLine
	call _FUNC_GetMemorySize
	
	;重置软盘
	INT13_RstDskSys [CONST_FAT12_BS_DrvNum]

LABEL_SEARCHING_LOADER:
	;判断根目录所有扇区是否均被找完
	cmp word [DW_LOADER_ROOTDIR_SIZE],0
	dec word [DW_LOADER_ROOTDIR_SIZE]
	jz LABEL_LOADER_MISSING
	;设输出地址(ES:BX) [INT13输出]
	mov ax, CONST_KERNEL_BaseAddr
	mov es, ax
	mov bx, CONST_KERNEL_OffsetAddr
	mov ax,[DW_LOADER_LOADING_SECTOR]
	mov ch,1
	
	call _FUNC_ReadSector
	
	;ES:DI为偏移地址
	mov di,CONST_KERNEL_OffsetAddr
	cld
	;一个扇区有512字节(根据BootDefinitions.inc)，一个条目占用32字节,故有16条目
	mov dx,10h
LABEL_COMPARE_ENTRY:
	;设DS:SI为串地址
	mov si,CONST_LOADER_KrnlFile
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
	mov cx,CONST_KERNEL_BaseAddr
	mov es,cx
	mov bx,CONST_KERNEL_OffsetAddr

LABEL_LOAD_READ:
	
	;读取第一个扇区
	
	mov cx, 0x0100
	call _FUNC_ReadSector
	pop ax
	
	call _FUNC_GetFATEntry
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
	inc word [DW_LOADER_LOADING_SECTOR]
	jmp LABEL_SEARCHING_LOADER

LABEL_FILE_LOADED:
	;完成加载
	call _FUNC_DisplayNewLine
	mov cx, CONST_LOADER_Loader_KrnlLoaded_LEN
	mov bp, CONST_LOADER_Loader_KrnlLoaded
	call _FUNC_DisplayString
	
	;清屏
	INT10_ScrollUp 0, CONST_BINT_WHITE_FG|CONST_BINT_BLACK_BG, 0x0, 0xffff

	;Protected Mode 准备
	
	push dx
	mov	dx, 03F2h
	mov	al, 0
	out	dx, al
	pop	dx

	lgdt [PTR_GDTPTR]
	cli
	in	al, 92h
	or	al, 00000010b
	out	92h, al
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax
	
	jmp	dword SELECTOR_GENERAL:(CONST_LOADER_BasePhyAddr+LABEL_PROGRAM_PROTECTMODE_START)

	;jmp CONST_KERNEL_BaseAddr:CONST_KERNEL_OffsetAddr

LABEL_LOADER_MISSING:
	;根目录所有扇区均被找完，没有发现Loader
	mov cx, CONST_LOADER_Loader_KrnlMissing_LEN
	mov bp, CONST_LOADER_Loader_KrnlMissing
	call _FUNC_DisplayString
	

	;主过程结束
	jmp $




;实模式下的过程
;由于调用BIOS中断，下列函数可能无法在保护模式中执行
FUNCTIONS_REAL_MODE_1:


;函数 DisplayInt - 显示数字(通过INT10h)
;参数:	cx 数字长度
;参数	bp 偏移地址
;(调整起始位置通过 DisplaySpace进行)
_FUNC_DisplayInt:
	SaveRegs
	mov ax,ds
	mov es,ax
	mov [DB_DISPLAY_INTLEN],cl
	xor ch,ch
	mov ax,bp
	shr cx,1
	add ax,cx
	mov bp,ax
	shl cx,1
	mov dl,byte[DB_DISPLAY_PRINTCOL]
	push ax
	mov al,dl
	add al,cl
	mov byte[DB_DISPLAY_PRINTCOL],al
	pop ax
_LABEL_FUNC_DisplayInt_Loop:
	mov cl, byte [DB_DISPLAY_INTLEN]
	xor ch,ch
	jcxz _LABEL_FUNC_DisplayInt_End
	dec cx
	mov byte [DB_DISPLAY_INTLEN],cl
	inc cl
	and cl,1
	jz _LABEL_FUNC_DisplayInt_E
	jmp _LABEL_FUNC_DisplayInt_O
_LABEL_FUNC_DisplayInt_E:
	dec bp
	mov al,byte[es:bp]
	and al,0xf0 
	shr al,4
	jmp _LABEL_FUNC_DisplayInt_Cont
_LABEL_FUNC_DisplayInt_O:
	mov al,byte[es:bp]
	and al,0x0f
_LABEL_FUNC_DisplayInt_Cont:
	cmp al,0xA
	jnb _LABEL_FUNC_DisplayInt_LargerThan9
	jmp _LABEL_FUNC_DisplayInt_SmallerThan10
_LABEL_FUNC_DisplayInt_LargerThan9:
	
	add al,0x57
	jmp _LABEL_FUNC_DisplayInt_Print
_LABEL_FUNC_DisplayInt_SmallerThan10:
	
	add al,0x30
	jmp _LABEL_FUNC_DisplayInt_Print
_LABEL_FUNC_DisplayInt_Print:
	mov [DB_DISPLAY_INTTMP],al
	push bp
	mov bp,DB_DISPLAY_INTTMP
	mov cx,1
	mov dh,[DB_DISPLAY_PRINTLINE]
	INT10_DispStr 0,1,CONST_BINT_WHITE_FG|CONST_BINT_BLACK_BG
	inc dl
	pop bp
	jmp _LABEL_FUNC_DisplayInt_Loop
_LABEL_FUNC_DisplayInt_End:
	;inc byte[DB_DISPLAY_PRINTLINE]
	RecoverRegs
	ret


;函数 DisplayString - 显示字符串
;参数:	cx 字符串长度(CL有效)
;		bp 字符串偏移地址	
_FUNC_DisplayString:
	mov ax,ds
	mov es,ax
	xor dx,dx
	mov dh,[DB_DISPLAY_PRINTLINE]
	mov dl,[DB_DISPLAY_PRINTCOL]
	mov al,dl
	add al,cl
	mov [DB_DISPLAY_PRINTCOL],al
	INT10_DispStr 0,1,CONST_BINT_WHITE_FG|CONST_BINT_BLACK_BG
	;inc dh
	mov [DB_DISPLAY_PRINTLINE],dh
	ret

;函数 GetMemorySize - 获取内存大小
;改变:	bp	在DisplayString中被改变
_FUNC_GetMemorySize:
	mov ax,ds
	mov es,ax
	mov di, DBF_MEMORY_ARDS_BUFFER
	push cx
	push bp
	mov cx,CONST_LOADER_GetMemory_IntroHeader_LEN
	mov bp,CONST_LOADER_GetMemory_IntroHeader
	call _FUNC_DisplayString
	call _FUNC_DisplayNewLine
	pop bp
	pop cx
_LABEL_FUNC_GetMemorySize_START:
	INT15_QuerySMAP dword [DD_MEMORY_CONTINUATION], 256
	jc _LABEL_FUNC_GetMemorySize_FAILURE
	cmp ebx,0
	jz _LABEL_FUNC_GetMemorySize_END
	mov dword [DD_MEMORY_CONTINUATION], ebx
	push cx
	push bp

	;基地址低32位
	mov cx,8
	mov bp,DBF_MEMORY_ARDS_BUFFER
	call _FUNC_DisplayInt
	call _FUNC_DisplaySpace 
	;基地址高32位
	mov cx,8
	mov bp,DBF_MEMORY_ARDS_BUFFER+4
	call _FUNC_DisplayInt
	call _FUNC_DisplaySpace 
	;界限低32位
	mov cx,8
	mov bp,DBF_MEMORY_ARDS_BUFFER+8
	call _FUNC_DisplayInt
	call _FUNC_DisplaySpace 
	;界限高32位
	mov cx,8
	mov bp,DBF_MEMORY_ARDS_BUFFER+12
	call _FUNC_DisplayInt
	call _FUNC_DisplaySpace 
	;类型
	mov cx,8
	mov bp,DBF_MEMORY_ARDS_BUFFER+16
	call _FUNC_DisplayInt
	call _FUNC_DisplaySpace 

	cmp word[DBF_MEMORY_ARDS_BUFFER+16],1
	jnz _LABEL_FUNC_GetMemorySize_1
	push eax
	mov eax,[DBF_MEMORY_ARDS_BUFFER]
	add eax,[DBF_MEMORY_ARDS_BUFFER+8]
	cmp eax,[DD_MEMORY_SIZE]
	jb _LABEL_FUNC_GetMemorySize_2
	mov [DD_MEMORY_SIZE], eax
	
_LABEL_FUNC_GetMemorySize_2:
	pop eax

_LABEL_FUNC_GetMemorySize_1:
	call _FUNC_DisplayNewLine
	pop bp
	pop cx
	jmp _LABEL_FUNC_GetMemorySize_START

_LABEL_FUNC_GetMemorySize_END:
	mov cx,CONST_LOADER_GetMemory_Success_LEN
	mov bp,CONST_LOADER_GetMemory_Success
	call _FUNC_DisplayString
	ret

_LABEL_FUNC_GetMemorySize_FAILURE:
	mov cx,CONST_LOADER_GetMemory_Failure_LEN
	mov bp,CONST_LOADER_GetMemory_Failure
	call _FUNC_DisplayString
	ret

;函数 VideoMdTest - 图形模式测试
_FUNC_VideoMdTest:
	INT10_SetVideoMode 0x13
	INT10_GetVideoMode
	mov byte [DB_BINT_VIDEO_ACTIVE_PAGE], bh
	mov bl, 0xff
LABEL_TEST:
	xor bh,bh
	mov word[DW_I],bx
	INT10_WrPixel CONST_BINT_RED_FG,0,word[DW_I],10
	dec bl
	cmp bl,0
	jnz LABEL_TEST
	
	jmp $
	ret

;函数 GetCursorStat - 获取光标位置
_FUNC_GetCursorStat:
	SaveRegs
	RecoverRegs
	ret

;函数 DisplayNewLine - 换行(不显示效果)
_FUNC_DisplayNewLine:
	inc byte[DB_DISPLAY_PRINTLINE]
	mov byte[DB_DISPLAY_PRINTCOL],0 
	ret

;函数 DisplaySpace - 打印空格
_FUNC_DisplaySpace:
	SaveRegs
	mov cx,CONST_LOADER_Disp_Space_LEN
	mov bp,CONST_LOADER_Disp_Space
	call _FUNC_DisplayString
	RecoverRegs
	ret

;函数 ReadSector
;参数:	es:bx 接收读取内容的地址(由INT 13H决定)
;		ch 读取扇区数
;		ax 起始扇区号
_FUNC_ReadSector:
	;保存寄存器
	SaveRegs
	and eax,0x0000FFFF
	and ecx,0x0000FFFF
	and ebx,0x0000FFFF
	and edx,0x0000FFFF
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
	RecoverRegs
	ret

;函数 GetFATEntry
;参数	ax FAT编号
;REFERENCE: Forrest Yu
_FUNC_GetFATEntry:
	push es
	push bx
	mov bx, CONST_LOADER_BaseAddr 
	sub bx, 0100h  
	mov es, bx   
	mov byte [DB_LOADER_ODD_FLAG], 0
	mov bx, 3
	mul bx   
	mov bx, 2
	div bx   
	cmp dx, 0
	jz _LABEL_FUNC_GetFATEntry_Even
	mov byte [DB_LOADER_ODD_FLAG], 1
_LABEL_FUNC_GetFATEntry_Even:
	xor dx, dx   
	mov bx, [CONST_FAT12_BPB_BytsPerSec]
	div bx   
	push dx
	xor bx,bx  
	add	ax, 1 ;根据BootDefinitions.inc设定
	mov cx, 0x0200
	call _FUNC_ReadSector  
	pop dx
	add bx, dx
	mov ax, [es:bx]
	cmp byte [DB_LOADER_ODD_FLAG], 1
	jnz _LABEL_FUNC_GetFATEntry_Even_2
	shr ax, 4
_LABEL_FUNC_GetFATEntry_Even_2:
	and ax, 0FFFh
_LABEL_FUNC_GetFATEntry_Done:
	 pop bx
	 pop es
	 ret

;---------------------------------- 保护模式 PROTECTED MODE ---------------------------------------------
LABEL_PROTECTED_MODE:

[SECTION .s32]
ALIGN	32
[BITS	32]
LABEL_PROGRAM_PROTECTMODE_START:
	;GS寄存器将指向显存Selector
	mov ax,SELECTOR_CGA_VIDEO
	mov gs,ax

	;初始化段寄存器
	mov ax,SELECTOR_GENERAL_DATA
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov ss,ax
	mov esp, EQP_STACKTOP
	
	;坐标重置
	call FUNC_DisplayResetPM

	mov bx,0x0404
	mov ah, 0xFF
	mov al, 'R'
	call FUNC_CGADisplayCharPM



	call FUNC_DisplayNewLinePM

	;坐标重置
	call FUNC_DisplayResetPM

	mov ah, 0Fh
	mov al,8
	
	mov ebp,DDP_TEST
	
	call FUNC_CGADisplayIntPM


	call FUNC_DisplayNewLinePM

	mov ah, 0Fh
	mov al,8
	mov ebp,DDP_MEMORY_SIZE
	call FUNC_CGADisplayIntPM

	call FUNC_DisplayNewLinePM

	mov ah, 2Fh
	mov ebp,DSP_TEST
	call FUNC_CGADisplayStringPM

	BREAK_PT
	call FUNC_MemoryPaging
	

	call FUNC_DisplayNewLinePM
	mov ah, 2Fh
	mov ebp,DSP_TEST
	call FUNC_CGADisplayStringPM

	call FUNC_RearrangeKernel


	BREAK_PT
	jmp dword SELECTOR_GENERAL:CONST_KERNEL_Entry



;函数: CGADisplayCharPM [保护模式]: 在CGA显存中写入字符
;参数：AH 颜色常量(仍可用BIOSInterrupt.inc中替代)
;	   AL 字符内容
;	   BH 行
;      BL 列
FUNC_CGADisplayCharPM:
	SaveRegs32
	push eax
	xor eax,eax
	xor edi,edi
	mov al,bh
	mov cx,CONST_VIDEO_CGA_COLS
	mul cx
	xor cx,cx
	mov cl,bl
	add ax,cx
	mov di,ax
	pop eax
	mov [gs:edi],ax
	RecoverRegs32
	ret

;函数 CGADisplayIntPM [保护模式]: 在CGA显存中写入一个数字
;参数： AH 颜色常量
;		AL 数字长度（1个字节算2个长度单位）
;		ES 数据段选择子
;		EBP 偏移地址
FUNC_CGADisplayIntPM:
	SaveRegs32
	;计算起始地址 EDI
	push eax
	push ebx
	mov al,[DBP_DISPLAY_PRINTLINE]
	xor ah,ah
	mov bx,CONST_VIDEO_CGA_COLS
	mul bx
	xor bx,bx
	mov bl,[DBP_DISPLAY_PRINTCOL]
	add ax,bx
	add ax,ax
	xor edi,edi
	mov di,ax
	pop ebx
	pop eax

	;将指针EBP置于数字高位
	push eax
	shr al,1
	mov bl,al
	shl al,1
	mov eax,ebp
	add al,bl
	dec eax
	mov ebp,eax
	pop eax
	push ax
	xor ah,ah
	mov cx,ax
	pop ax

	;更新下一次函数调用的位置
	push cx
	push ax
	mov al,[DBP_DISPLAY_PRINTCOL]
	add al,cl
	mov [DBP_DISPLAY_PRINTCOL],al
	pop ax
	pop cx
	
LABEL_FUNC_CGADisplayIntPM_Loop:
	jcxz LABEL_FUNC_CGADisplayIntPM_End;
	mov bx,cx
	and bx,1
	cmp bx,1
	jnz LABEL_FUNC_CGADisplayIntPM_1;高位
	jmp LABEL_FUNC_CGADisplayIntPM_2;低位

LABEL_FUNC_CGADisplayIntPM_1:
	mov bl,byte[es:ebp]
	
	and bl,0x00ff
	shr bl,4
	
	cmp bl,0xA
	jnb LABEL_FUNC_CGADisplayIntPM_Larger
	jmp LABEL_FUNC_CGADisplayIntPM_Smaller

LABEL_FUNC_CGADisplayIntPM_2:
	mov bl,byte[es:ebp]
	and bl,0x000f
	cmp bl,0xA
	
	dec ebp
	jnb LABEL_FUNC_CGADisplayIntPM_Larger
	jmp LABEL_FUNC_CGADisplayIntPM_Smaller

LABEL_FUNC_CGADisplayIntPM_Larger:
	add bl,0x57
	jmp LABEL_FUNC_CGADisplayIntPM_Print
LABEL_FUNC_CGADisplayIntPM_Smaller:
	add bl,0x30
LABEL_FUNC_CGADisplayIntPM_Print:
	mov al,bl
	mov [gs:edi],ax
	
	add edi,2
	dec cx
	jmp LABEL_FUNC_CGADisplayIntPM_Loop
LABEL_FUNC_CGADisplayIntPM_End:
	RecoverRegs32
	ret

;函数 CGADisplayStringPM [保护模式] - 显示字符串(0作结尾)
;参数： AH 颜色常量
;		ES 数据段选择子
;		EBP 偏移地址
FUNC_CGADisplayStringPM:
	SaveRegs32
	;计算EDI
	push ax
	xor eax,eax
	mov al,[DBP_DISPLAY_PRINTLINE]
	mov bx,CONST_VIDEO_CGA_COLS
	mul bx
	xor cx,cx
	mov cl,[DBP_DISPLAY_PRINTCOL]
	add ax,cx
	add eax,eax
	mov edi,eax
	pop ax
	mov dx,0
LABEL_FUNC_CGADisplayStringPM_Loop:
	mov al,[es:ebp]
	cmp al,0
	jz LABEL_FUNC_CGADisplayStringPM_End
	mov [gs:edi],ax
	inc dx
	inc ebp
	add edi,2
	jmp LABEL_FUNC_CGADisplayStringPM_Loop
LABEL_FUNC_CGADisplayStringPM_End:
	mov [DBP_DISPLAY_PRINTCOL],dl
	RecoverRegs32
	ret


;函数 DisplayNewLinePM [保护模式] - 换行(不显示效果)
FUNC_DisplayNewLinePM:
	inc byte[DBP_DISPLAY_PRINTLINE]
	mov byte[DBP_DISPLAY_PRINTCOL],0 
	ret

;函数 DisplayResetPM [保护模式] - 重置输出坐标
FUNC_DisplayResetPM:
	mov byte[DBP_DISPLAY_PRINTLINE],0
	mov byte[DBP_DISPLAY_PRINTCOL],0 
	ret



;函数 MemoryCopy [保护模式] - 内存复制
;参数：	PUSH (dd)目标指针
;		PUSH (dd)来源指针
;		PUSH (dd)复制大小
;		ES	 数据段选择子
;		DS   数据段选择子
;返回:	EAX  0
;C：		DWORD MemoryCopy(DWORD destPtr,DWORD sourcePtr,DWORD size)
FUNC_MemoryCopy:
	push ebp
	mov ebp,esp
	push esi
	push edi
	push ecx
	;复制过程[es:edi]->[ds:esi]

	mov esi,[ebp+8] ;DEST, 前面还有EIP和EBP
	mov edi,[ebp+12];SRC
	mov ecx,[ebp+16]
LABEL_FUNC_MemoryCopy_Loop:
	push eax
	mov eax,[es:edi]
	inc edi
	mov [ds:esi],eax
	inc esi
	loop LABEL_FUNC_MemoryCopy_Loop
	pop ecx
	pop edi
	pop esi
	mov esp,ebp
	pop ebp
	xor eax,eax
	ret 

;函数 MemoryPaging [保护模式] - 内存分页
;参考: Forrest Yu
FUNC_MemoryPaging:
	xor	edx, edx
	mov	eax, [DD_MEMORY_SIZE]
	mov	ebx, 400000h	
	div	ebx
	mov	ecx, eax	
	test edx, edx
	jz	.no_remainder
	inc	ecx		
.no_remainder:
	push ecx		
	mov	ax, SELECTOR_GENERAL_DATA
	mov	es, ax
	mov	edi, CONST_MEMORY_PageDirBase	
	xor	eax, eax
	mov	eax, CONST_MEMORY_PageTblBase | CONST_MEMORYPG_P  | CONST_MEMORYPG_USU | CONST_MEMORYPG_RWW
.1:
	stosd
	add	eax, 4096		
	loop .1
	pop	eax			
	mov	ebx, 1024		
	mul	ebx
	mov	ecx, eax		
	mov	edi, CONST_MEMORY_PageTblBase	
	xor	eax, eax
	mov	eax, CONST_MEMORYPG_P  | CONST_MEMORYPG_USU | CONST_MEMORYPG_RWW
.2:
	stosd
	add	eax, 4096		
	loop .2
	mov	eax, CONST_MEMORY_PageDirBase
	mov	cr3, eax
	mov	eax, cr0
	or eax, 80000000h
	mov	cr0, eax
	jmp	short .3
.3:
	nop
	ret

;函数 RearrangeKernel: 重新整理内核
;参考: Forrest Yu
FUNC_RearrangeKernel:
	xor	esi, esi
	mov	cx, word [CONST_KERNEL_BasePhyAddr + 2Ch]; 
	movzx	ecx, cx
	mov	esi, [CONST_KERNEL_BasePhyAddr + 1Ch]	
	add	esi, CONST_KERNEL_BasePhyAddr
.Begin:
	mov	eax, [esi + 0]
	cmp	eax, 0		
	jz	.NoAction
	push	dword [esi + 010h]		
	mov	eax, [esi + 04h]		
	add	eax, CONST_KERNEL_BasePhyAddr
	push eax
	push dword [esi + 08h]
	call FUNC_MemoryCopy
	add	esp, 12	
.NoAction:
	add	esi, 020h
	dec	ecx
	jnz	.Begin
	ret