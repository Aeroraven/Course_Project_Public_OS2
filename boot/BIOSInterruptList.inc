; ---------------------------------------------------------------			
; BIOSInterruptList.inc
; ---------------------------------------------------------------			
; Macros for BIOS Interrupt Calls (x86)
; x86 BIOS 中断调用宏
; By Aeroraven, 2021-3-12
; --------------------------------------------------------------
; 参考: https://www.cs.cmu.edu/~ralf/interrupt-list/inter61a.zip
; ---------------------------------------------------------------


;----------------------------------			
; 颜色常量
;----------------------------------
CONST_BINT_BLACK_FG			equ 0x0
CONST_BINT_BLUE_FG			equ 0x1
CONST_BINT_GREEN_FG			equ 0x2
CONST_BINT_RED_FG			equ 0x4
CONST_BINT_INTENSITY_FG		equ 0x8
CONST_BINT_WHITE_FG			equ 0xF

CONST_BINT_BLACK_BG			equ 0x0
CONST_BINT_BLUE_BG			equ 0x10
CONST_BINT_GREEN_BG			equ 0x20
CONST_BINT_RED_BG			equ 0x40
CONST_BINT_INTENSITY_BG		equ 0x80
CONST_BINT_WHITE_BG			equ 0xF0

;----------------------------------			
; 中断 10h: 显示服务
;----------------------------------

; [INT10h/00] : 设置显示模式
; 参数: VideoMode
; 返回: NULL
%macro INT10_SetVideoMode 1
	mov ah,00h
	mov al,%1
	int 10h
%endmacro


; [INT10h/01] : 设置光标形状
; 参数: 行 (4bit)
;		列 (4bit)
; 返回: NULL
%macro INT10_SetCursorShape 2
	mov ah,01h
	mov ch,%1
	mov cl,%2
	int 10h
%endmacro


; [INT10h/02] : 设置光标位置 (Text Mode)
; 参数: 页码 
;		行 
;		列 
; 返回: NULL
%macro INT10_SetCursorPos 3
	mov ah,02h
	mov bh,%1
	mov ch,%2
	mov cl,%3
	int 10h
%endmacro

;[INT10h/03]: 获取光标位置
; 参数：BH 页码
; 返回：DH 行
;		DL 列
%macro INT10_GetCursorPos 1
	mov bh,%1
	int 10h
%endmacro

; [INT10h/06] : 向上滚屏
; 参数: 滚屏行数 (0 - 清屏)
;		填充属性
;		左上坐标
;		右下坐标
; 返回: NULL
%macro INT10_ScrollUp 4
	mov ax,(0600h | %1)
	mov bh,%2
	mov cx,%3
	mov dx,%4
	int 10h
%endmacro

; [INT10h/07] : 向下滚屏
; 参数: 滚屏行数 (0 - 清屏)
;		填充属性
;		左上坐标
;		右下坐标
; 返回: NULL
%macro INT10_ScrollDown 4
	mov ah,07h
	mov al,%1
	mov bh,%2
	mov cx,%3
	mov dx,%4
	int 10h
%endmacro

; [INT10h/0F] : 获取视频模式
; 参数: NULL
; 返回: AL 视频模式
;		AH 字符列数
;		BH 活动显示页码
%macro INT10_GetVideoMode 0
	mov ah,0fh
	int 10h
%endmacro

; [INT10h/0C] : 填充图形像素
; 参数: AL	颜色
;		BH	显示页面
;		CX	X坐标
;		DX	Y坐标
; 返回: NULL
%macro INT10_WrPixel 4
	mov al, %1
	mov bh, %2
	mov cx, %3
	mov dx, %4
	mov ah, 0Ch
	int 10h
%endmacro


; [INT10h/13] : 显示字符串(TeletypeMode)
; 参数: 页码
;		显示输出方式
;		属性
;		x字符串长度
;		x坐标
; 条件: ES:BP 字符串起始地址
;		DX 坐标(DH,DL)
;		CX 长度
; 返回: NULL
%macro INT10_DispStr 3
	mov ax,(1300h | %2)
	mov bx,(%1<<16)|%3
	int 10h
%endmacro

;----------------------------------			
; 中断 13h: 低级硬盘设备服务
;----------------------------------

; [INT13h/00] : 重置硬盘设备
; 参数: 硬盘号
%macro INT13_RstDskSys 1
	mov ah, 00h
	mov dl, %1
	int 13h
%endmacro

; [INT13h/02] : 读取扇区
; 参数: 读取扇区数, 
;		柱面号, 
;		起始扇区号, 
;		磁头号, 
;		硬盘号
; 预设: ES:BX 输出地址
%macro INT13_RdSector 5
	mov ch,%2
	mov cl,%3
	mov dh,%4
	mov dl,%5
	mov ah,02h
	mov al,%1
	int 13h
%endmacro

; [INT13h/02] : 读取扇区(发生错误则重试)
; 参数: 读取扇区数 AL, 
;		柱面号 CH, 
;		起始扇区号 CL, 
;		磁头号 DH, 
;		硬盘号 DL
; 预设: ES:BX 输出地址
%macro INT13_RdSector2 5
	mov ch,%2
	mov cl,%3
	mov dh,%4
	mov dl,%5
%%INT13_RdSector_Retry:
	mov ah,02h
	mov al,%1
	int 13h
	jc %%INT13_RdSector_Retry
%endmacro

; [INT13h/02] : 读取扇区(发生错误则重试)
; 参数: 	读取扇区数
; 预设: ES:BX 输出地址
%macro INT13_RdSector3 1
%%INT13_RdSector_Retry3:
	mov ah,02h
	mov al,%1
	;xchg bx,bx
	int 13h
	;xchg bx,bx
	jc %%INT13_RdSector_Retry3
%endmacro

;----------------------------------			
; 中断 15h: 杂项
;----------------------------------

;[INT15/E8-20] ：查询系统地址映射
; 参数:	EBX 继续值(初始查询为0)
;		ECX 缓冲区大小
; 条件: ES:DI 缓冲区地址
; 输出:	CF	为零表示无错误
;		EAX	签名SMAP
;		ECX 缓冲区大小
;		EBX 连续值 (0或CF表示最后一个ARD)
;		[ES:DI]	ARD内容
%macro INT15_QuerySMAP 2
	mov eax, 0e820h
	mov edx, 0534d4150h
	mov ebx, %1
	mov ecx, %2
	int 15h
%endmacro