; ---------------------------------------------------------------			
; Loader.asm
; ---------------------------------------------------------------	
; Boot Loader
; ???????? Loader
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
	
	DBF_VESA_INFO_BUFFER:			
	VESA_ModeAttributes dw 0 ; mode attributes
	VESA_WinAAttributes db 0 ; window A attributes
	VESA_WinBAttributes db 0 ; window B attributes
	VESA_WinGranularity dw 0 ; window granularity
	VESA_WinSize dw 0 ; window size
	VESA_WinASegment dw 0 ; window A start segment
	VESA_WinBSegment dw 0 ; window B start segment
	VESA_WinFuncPtr dd 0 ; real mode pointer to window function
	VESA_BytesPerScanLine dw 0 ; bytes per scan line
	VESA_XResolution dw 0 ; horizontal resolution in pixels or characters3
	VESA_YResolution dw 0 ; vertical resolution in pixels or characters
	VESA_XCharSize db 0 ; character cell width in pixels
	VESA_YCharSize db 0 ; character cell height in pixels
	VESA_NumberOfPlanes db 0 ; number of memory planes
	VESA_BitsPerPixel db 0 ; bits per pixel
	VESA_NumberOfBanks db 0 ; number of banks
	VESA_MemoryModel db 0 ; memory model type
	VESA_BankSize db 0 ; bank size in KB
	VESA_NumberOfImagePages db 0 ; number of images
	VESA_Reserved db 1 ; reserved for page function
	VESA_RedMaskSize db 0 ; size of direct color red mask in bits
	VESA_RedFieldPosition db 0 ; bit position of lsb of red mask
	VESA_GreenMaskSize db 0 ; size of direct color green mask in bits
	VESA_GreenFieldPosition db 0 ; bit position of lsb of green mask
	VESA_BlueMaskSize db 0 ; size of direct color blue mask in bits
	VESA_BlueFieldPosition db 0 ; bit position of lsb of blue mask
	VESA_RsvdMaskSize db 0 ; size of direct color reserved mask in bits
	VESA_RsvdFieldPosition db 0 ; bit position of lsb of reserved mask
	VESA_DirectColorModeInfo db 0 ; direct color mode attributes
	VESA_PhysBasePtr dd 0 ; physical address for flat memory frame buffer
	VESA_Reserved1 dd 0 ; Reserved - always set to 0
	VESA_Reserved2 dw 0 ; Reserved - always set to 0
	VESA_LinBytesPerScanLine dw 0 ; bytes per scan line for linear modes
	VESA_BnkNumberOfImagePages db 0 ; number of images for banked modes
	VESA_LinNumberOfImagePages db 0 ; number of images for linear modes
	VESA_LinRedMaskSize db 0 ; size of direct color red mask (linear modes)
	VESA_LinRedFieldPosition db 0 ; bit position of lsb of red mask (linear modes)
	VESA_LinGreenMaskSize db 0 ; size of direct color green mask (linear modes)
	VESA_LinGreenFieldPosition db 0 ; bit position of lsb of green mask (linear modes)
	VESA_LinBlueMaskSize db 0 ; size of direct color blue mask (linear modes)
	VESA_LinBlueFieldPosition db 0 ; bit position of lsb of blue mask (linear modes)
	VESA_LinRsvdMaskSize db 0 ; size of direct color reserved mask (linear modes)
	VESA_LinRsvdFieldPosition db 0 ; bit position of lsb of reserved mask (linear modes)
	VESA_MaxPixelClock dd 0 ; maximum pixel clock (in Hz) for graphics mode
	DBF_MEMORY_ARDS_BUFFER_PLACEHOLDER:			times 256	db 0

	VESAQ_1	equ VESA_PhysBasePtr-DBF_VESA_INFO_BUFFER

VARIABLES_PROTECTED_MODE_1:
	DBP_DISPLAY_PRINTLINE			equ DB_DISPLAY_PRINTLINE + CONST_LOADER_BasePhyAddr
	DBP_DISPLAY_PRINTCOL			equ DB_DISPLAY_PRINTCOL + CONST_LOADER_BasePhyAddr

	DDP_TEST						equ DD_TEST + CONST_LOADER_BasePhyAddr
	DSP_TEST						equ DS_TEST + CONST_LOADER_BasePhyAddr

	EQP_STACKTOP					equ 0x100 + CONST_LOADER_BasePhyAddr
	DDP_MEMORY_SIZE					equ DD_MEMORY_SIZE + CONST_LOADER_BasePhyAddr
	

LABEL_GDT:
	DESC_EMPTY:						DESCRIPTOR 0, 0, 0
	DESC_GENERAL:					DESCRIPTOR 0, 0fffffffh, CONST_DAPS_GENERAL_R0
	DESC_GENERAL_DATA:				DESCRIPTOR 0, 0fffffffh, CONST_DAPS_GENERAL_DATA_R0
	DESC_CGA_VIDEO:					DESCRIPTOR CONST_VIDEOMEMORY_CGA_COLTEXT_BASE, CONST_VIDEOMEMORY_CGA_LIMIT, CONST_DAPS_CGA_VIDEO_R3
	DESC_VGA_VIDEO:					DESCRIPTOR CONST_VIDEOMEMORY_VGA_BASE, CONST_VIDEOMEMORY_VGA_LIMIT, CONST_DAPS_VGA_VIDEO_R3
	DESC_MDA_VIDEO:					DESCRIPTOR CONST_VIDEOMEMORY_MDA_BASE, CONST_VIDEOMEMORY_MDA_LIMIT, CONST_DAPS_MDA_VIDEO_R3
	DESC_VESA_VIDEO:				DESCRIPTOR CONST_VIDEOMEMORY_VESA_BASE, CONST_VIDEOMEMORY_VESA_LIMIT, CONST_DAPS_VESA_VIDEO_R3
	
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
	SELECTOR_VESA_VIDEO				equ DESC_VESA_VIDEO - LABEL_GDT + CONST_SA_RPL3
	
LABEL_PROGRAM_START:
	;??ʼ???μĴ?????ջ
	mov ax,cs
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax ;!!!!! ???ӱ?#BR?쳣 TAT
	mov sp,0100h

	INT10_ScrollUp 0, CONST_BINT_WHITE_FG|CONST_BINT_BLACK_BG, 0x0, 0xffff
	mov cx,CONST_LOADER_Loaded_LEN
	mov bp,CONST_LOADER_Loaded
	call _FUNC_DisplayString

	;FAT??ȡ????????ʼ??
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
	
	;????????
	INT13_RstDskSys [CONST_FAT12_BS_DrvNum]

LABEL_SEARCHING_LOADER:
	;?жϸ?Ŀ¼?????????Ƿ?????????
	cmp word [DW_LOADER_ROOTDIR_SIZE],0
	dec word [DW_LOADER_ROOTDIR_SIZE]
	jz LABEL_LOADER_MISSING
	;????????ַ(ES:BX) [INT13????]
	mov ax, CONST_KERNEL_BaseAddr
	mov es, ax
	mov bx, CONST_KERNEL_OffsetAddr
	mov ax,[DW_LOADER_LOADING_SECTOR]
	mov ch,1
	
	call _FUNC_ReadSector
	
	;ES:DIΪƫ?Ƶ?ַ
	mov di,CONST_KERNEL_OffsetAddr
	cld
	;һ????????512?ֽ?(????BootDefinitions.inc)??һ????Ŀռ??32?ֽ?,????16??Ŀ
	mov dx,10h
LABEL_COMPARE_ENTRY:
	;??DS:SIΪ????ַ
	mov si,CONST_LOADER_KrnlFile
	cmp dx,0
	jz LABEL_NEXT_SECTOR
	dec dx
	mov cx,0xb
LABEL_COMPARE_FILENAME:
	;?Ƚ??ļ???
	jcxz LABEL_FILE_FOUND
	dec cx
	lodsb
	cmp al,byte[es:di]
	;????ƥ????һ?ַ?
	jz LABEL_NEXT_CHAR
	;û?ҵ? - ???? inc di??Ӱ??(+11??di????Ӱ??11λ)
	and di,0ffe0h
	;diָ????һ??Ŀ
	add di,20h
	jmp LABEL_COMPARE_ENTRY

LABEL_NEXT_CHAR:
	;??һ?ַ?
	inc di
	jmp LABEL_COMPARE_FILENAME
	
LABEL_FILE_FOUND:
	
	;DIָ????ʼ?غ?
	and di, 0ffe0h
	add di, 0x1A
	;AX ָ???ļ???FAT???е???ʼ??Ŀ??Ӧ?ı???
	mov ax,word[es:di]
	push ax
	;AX ָ???ļ???ʼ??????
	add al, byte [CONST_FAT12_BS_RootDirSector]
	add al, byte [CONST_FAT12_BS_DeltaSectorNo]

	
	;ES:BX ָ?????ص?ַ
	mov cx,CONST_KERNEL_BaseAddr
	mov es,cx
	mov bx,CONST_KERNEL_OffsetAddr

LABEL_LOAD_READ:
	
	;??ȡ??һ??????
	
	mov cx, 0x0100
	call _FUNC_ReadSector
	pop ax
	
	call _FUNC_GetFATEntry
	;????FATָ????һ????λ0xFFF??˵???ѵ????ļ?β
	cmp ax,0x0FFF
	jz LABEL_FILE_LOADED

	push ax
	add al,  [CONST_FAT12_BS_RootDirSector]
	add al, [CONST_FAT12_BS_DeltaSectorNo]
	add bx, [CONST_FAT12_BPB_BytsPerSec]
	jmp LABEL_LOAD_READ

LABEL_NEXT_SECTOR:
	;??һ????
	inc word [DW_LOADER_LOADING_SECTOR]
	jmp LABEL_SEARCHING_LOADER

LABEL_FILE_LOADED:
	;???ɼ???
	call _FUNC_DisplayNewLine
	mov cx, CONST_LOADER_Loader_KrnlLoaded_LEN
	mov bp, CONST_LOADER_Loader_KrnlLoaded
	call _FUNC_DisplayString
	
	;????
	INT10_ScrollUp 0, CONST_BINT_WHITE_FG|CONST_BINT_BLACK_BG, 0x0, 0xffff


	; BREAK_PT
	;VESA VGA?Դ?????
	mov ax,0x4f02
	mov bx,CONST_VESA_DISPLAY_MODE
	int 10h
	mov cx,CONST_VESA_DISPLAY_MODE
	mov ax,ds
	mov es,ax
	mov di,DBF_VESA_INFO_BUFFER
	mov ax,0x4f01
	int 10h
	BgaWriteRegister VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED
	BgaWriteRegister VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED
	mov ax,di
	mov si,VESA_PhysBasePtr
	mov ecx,[ds:si]
	;xchg bx,bx
	add ax,4

	;Protected Mode ׼??
	
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
	;??Ŀ¼???????????????꣬û?з???Loader
	mov cx, CONST_LOADER_Loader_KrnlMissing_LEN
	mov bp, CONST_LOADER_Loader_KrnlMissing
	call _FUNC_DisplayString
	

	;?????̽???
	jmp $




;ʵģʽ?µĹ???
;???ڵ???BIOS?жϣ????к????????޷??ڱ???ģʽ??ִ??
FUNCTIONS_REAL_MODE_1:


;???? DisplayInt - ??ʾ????(ͨ??INT10h)
;????:	cx ???ֳ???
;????	bp ƫ?Ƶ?ַ
;(??????ʼλ??ͨ?? DisplaySpace????)
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


;???? DisplayString - ??ʾ?ַ???
;????:	cx ?ַ???????(CL??Ч)
;		bp ?ַ???ƫ?Ƶ?ַ	
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

;???? GetMemorySize - ??ȡ?ڴ???С
;?ı?:	bp	??DisplayString?б??ı?
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

	;????ַ??32λ
	mov cx,8
	mov bp,DBF_MEMORY_ARDS_BUFFER
	call _FUNC_DisplayInt
	call _FUNC_DisplaySpace 
	;????ַ??32λ
	mov cx,8
	mov bp,DBF_MEMORY_ARDS_BUFFER+4
	call _FUNC_DisplayInt
	call _FUNC_DisplaySpace 
	;???޵?32λ
	mov cx,8
	mov bp,DBF_MEMORY_ARDS_BUFFER+8
	call _FUNC_DisplayInt
	call _FUNC_DisplaySpace 
	;???޸?32λ
	mov cx,8
	mov bp,DBF_MEMORY_ARDS_BUFFER+12
	call _FUNC_DisplayInt
	call _FUNC_DisplaySpace 
	;????
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

;???? VideoMdTest - ͼ??ģʽ????
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

;???? GetCursorStat - ??ȡ????λ??
_FUNC_GetCursorStat:
	SaveRegs
	RecoverRegs
	ret

;???? DisplayNewLine - ????(????ʾЧ??)
_FUNC_DisplayNewLine:
	inc byte[DB_DISPLAY_PRINTLINE]
	mov byte[DB_DISPLAY_PRINTCOL],0 
	ret

;???? DisplaySpace - ??ӡ?ո?
_FUNC_DisplaySpace:
	SaveRegs
	mov cx,CONST_LOADER_Disp_Space_LEN
	mov bp,CONST_LOADER_Disp_Space
	call _FUNC_DisplayString
	RecoverRegs
	ret

;???? ReadSector
;????:	es:bx ???ն?ȡ???ݵĵ?ַ(??INT 13H????)
;		ch ??ȡ??????
;		ax ??ʼ??????
_FUNC_ReadSector:
	;?????Ĵ???
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
	div bl  ;??->AL;????->AH
	pop bx
	;CH=??????
	
	mov ch,al
	shr ch,1
	;DH=??ͷ??
	mov dh,al 
	and dh,1
	;CL=??ʼ??????
	inc ah
	mov cl,ah
	;DL=Ӳ?̺?
	
	xor dl,dl
	
	INT13_RdSector3 byte[bp-2]
	;?ָ?ջ
	add esp,2
	pop bp
	RecoverRegs
	ret

;???? GetFATEntry
;????	ax FAT????
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
	add	ax, 1 ;????BootDefinitions.inc?趨
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

;---------------------------------- ????ģʽ PROTECTED MODE ---------------------------------------------
LABEL_PROTECTED_MODE:

[SECTION .s32]
ALIGN	32
[BITS	32]
LABEL_PROGRAM_PROTECTMODE_START:
	;GS?Ĵ?????ָ???Դ?Selector
	mov ax,SELECTOR_VESA_VIDEO
	mov gs,ax

	;??ʼ???μĴ???
	mov ax,SELECTOR_GENERAL_DATA
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov ss,ax
	mov esp, EQP_STACKTOP
	
	;????????
	call FUNC_DisplayResetPM
	

	;?ڴ???ҳ
	call FUNC_MemoryPaging
	xchg bx,bx
	call FUNC_RearrangeKernel
	xchg bx,bx
	jmp dword SELECTOR_GENERAL:CONST_KERNEL_Entry

FUNC_VESATest:
	;xchg bx,bx
	mov ecx,0xcfffffff
	mov edi,0
testHere:
	inc edi
	mov ax,0xbbbb
	mov [gs:edi],ax
	jmp testHere

	jmp $


;????: CGADisplayCharPM [????ģʽ]: ??CGA?Դ???д???ַ?
;??????AH ??ɫ????(?Կ???BIOSInterrupt.inc??????)
;	   AL ?ַ?????
;	   BH ??
;      BL ??
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

;???? CGADisplayIntPM [????ģʽ]: ??CGA?Դ???д??һ??????
;?????? AH ??ɫ????
;		AL ???ֳ??ȣ?1???ֽ???2?????ȵ?λ??
;		ES ???ݶ?ѡ????
;		EBP ƫ?Ƶ?ַ
FUNC_CGADisplayIntPM:
	SaveRegs32
	;??????ʼ??ַ EDI
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

	;??ָ??EBP???????ָ?λ
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

	;??????һ?κ??????õ?λ??
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
	jnz LABEL_FUNC_CGADisplayIntPM_1;??λ
	jmp LABEL_FUNC_CGADisplayIntPM_2;??λ

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

;???? CGADisplayStringPM [????ģʽ] - ??ʾ?ַ???(0????β)
;?????? AH ??ɫ????
;		ES ???ݶ?ѡ????
;		EBP ƫ?Ƶ?ַ
FUNC_CGADisplayStringPM:
	SaveRegs32
	;????EDI
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


;???? DisplayNewLinePM [????ģʽ] - ????(????ʾЧ??)
FUNC_DisplayNewLinePM:
	inc byte[DBP_DISPLAY_PRINTLINE]
	mov byte[DBP_DISPLAY_PRINTCOL],0 
	ret

;???? DisplayResetPM [????ģʽ] - ????????????
FUNC_DisplayResetPM:
	mov byte[DBP_DISPLAY_PRINTLINE],0
	mov byte[DBP_DISPLAY_PRINTCOL],0 
	ret



;???? MemoryCopy [????ģʽ] - ?ڴ渴??
;??????	PUSH (dd)Ŀ??ָ??
;		PUSH (dd)??Դָ??
;		PUSH (dd)???ƴ?С
;		ES	 ???ݶ?ѡ????
;		DS   ???ݶ?ѡ????
;????:	EAX  0
;C??		DWORD MemoryCopy(DWORD destPtr,DWORD sourcePtr,DWORD size)

FUNC_MemoryCopy:
	push ebp
	mov ebp,esp
	push esi
	push edi
	push ecx
	mov edi,[ebp+8] ;DEST, ǰ?滹??EIP??EBP
	mov esi,[ebp+12];SRC
	mov ecx,[ebp+16]
	
LABEL_FUNC_MemoryCopy_Loop:
	mov al, [ds:esi]
	inc esi
	mov byte [es:edi],al
	inc edi
	loop LABEL_FUNC_MemoryCopy_Loop
	mov eax,[ebp+8]
	pop ecx
	pop esi
	pop edi
	mov esp,ebp
	pop ebp
	ret 

;???? MemoryPaging [????ģʽ] - ?ڴ???ҳ
;?ο?: Forrest Yu
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


;???? RearrangeKernel: ?????????ں?
;?ο?: Forrest Yu
FUNC_RearrangeKernel:
	xor	esi, esi
	mov	cx, word [CONST_KERNEL_BasePhyAddr + 2Ch]; ?? ecx <- pELFHdr->e_phnum
	movzx	ecx, cx					; ??
	mov	esi, [CONST_KERNEL_BasePhyAddr + 1Ch]	; esi <- pELFHdr->e_phoff
	add	esi, CONST_KERNEL_BasePhyAddr		; esi <- OffsetOfKernel + pELFHdr->e_phoff
.Begin:
	mov	eax, [esi + 0]
	cmp	eax, 0				; PT_NULL
	jz	.NoAction
	push	dword [esi + 010h]		; size	??
	mov	eax, [esi + 04h]		;	??
	add	eax, CONST_KERNEL_BasePhyAddr	;	?? ::memcpy(	(void*)(pPHdr->p_vaddr),
	push	eax				; src	??		uchCode + pPHdr->p_offset,
	push	dword [esi + 08h]		; dst	??		pPHdr->p_filesz;
	call	FUNC_MemoryCopy				;	??
	add	esp, 12				;	??
.NoAction:
	add	esi, 020h			; esi += pELFHdr->e_phentsize
	dec	ecx
	jnz	.Begin

	ret