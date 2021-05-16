; ---------------------------------------------------------------   
; Kernel.asm
; --------------------------------------------------------------- 
; Operating System Kernel
; OS�ں�
; By Aeroraven, 2021-3-16
; --------------------------------------------------------------


extern kernel_start
extern exception_handler
extern spurious_interrupt_request
extern ProcessReady
extern tss
extern Ticks
extern KRNL_VESAFont_Row
extern KRNL_VESAFont_Col
extern K_IntReenter
extern irq_handler
extern syscall_table
;��
%macro  spurious_int_placeholder    1
        push    %1
        call    spurious_interrupt_request
        add     esp, 4
        hlt
%endmacro

%macro save_regs 0
    pushad
    push ds
    push es
    push fs
    push gs
%endmacro

%macro recover_regs 0
    pop gs
    pop fs
    pop es
    pop ds
    popad
%endmacro

%macro interrupt_master 1
    call Save
    in al,0x21
    or al,(1<<%1)
    out 0x21,al
    mov al,0x20
    out 0x20,al
    sti
    push %1
    call [irq_handler+4*%1]
    pop ecx
    cli
    in al,0x21
    and al,~(1<<%1)
    out 0x21,al
    ret
%endmacro

%macro interrupt_slave 1
    call Save
    in al, 0xA1 
    or al, (1 << (%1 - 8)) 
    out 0xA1, al 
    mov al, 0x20  
    out 0x20, al  
    nop    
    out 0xA0, al  
    sti 
    push %1 
    call [irq_handler + 4 * %1] 
    pop ecx   
    cli
    in al, 0xA1
    and al, ~(1 << (%1 - 8)) 
    out 0xA1, al 
    ret
%endmacro
;����
PROC_STACKTOP  equ 72
TSS3_S_SP0  equ 4
PROC_LDT_SELECTOR equ 72

;��ʼ

[SECTION .bss]
StackSpace  resb 2 * 1024
StackTop:  

[SECTION .text]

global _start
global Restart

_start: 
    mov esp, StackTop
    
    call kernel_start
    jmp $
    
Restart:
    mov esp,[ProcessReady]
    mov eax,[esp+72]
    
    lldt [esp+72]
    lea eax,[esp+PROC_STACKTOP]
    mov dword [tss+TSS3_S_SP0],eax

Restart_Reenter:
    dec dword[K_IntReenter]
    recover_regs
    add esp,4
    iretd
;�ж�
global AFE_EXCEPTION_DE
global AFE_EXCEPTION_DB
global AFE_EXCEPTION_NMI
global AFE_EXCEPTION_BP
global AFE_EXCEPTION_OF
global AFE_EXCEPTION_BR
global AFE_EXCEPTION_UD
global AFE_EXCEPTION_NM
global AFE_EXCEPTION_DF
global AFE_EXCEPTION_MF
global AFE_EXCEPTION_TS
global AFE_EXCEPTION_NP
global AFE_EXCEPTION_SS
global AFE_EXCEPTION_GP
global AFE_EXCEPTION_PF
global AFE_EXCEPTION_RSVD
global AFE_EXCEPTION_MF2
global AFE_EXCEPTION_AC
global AFE_EXCEPTION_MC
global AFE_EXCEPTION_XM

global AFE_INT_0
global AFE_INT_1
global AFE_INT_2
global AFE_INT_3
global AFE_INT_4
global AFE_INT_5
global AFE_INT_6
global AFE_INT_7
global AFE_INT_8
global AFE_INT_9
global AFE_INT_10
global AFE_INT_11
global AFE_INT_12
global AFE_INT_13
global AFE_INT_14
global AFE_INT_15
global AFE_INT_16

global SystemCall
global SYSCALL_GetTick
global SYSCALL_ConWrite
global SYSCALL_ConWriteX
global SYSCALL_SendRec

AFE_EXCEPTION_DE:
    push 0x0
    push 0
    jmp exception_handle
AFE_EXCEPTION_DB:
    push 0x0
    push 1
    jmp exception_handle
AFE_EXCEPTION_NMI:
    push 0x0
    push 2
    jmp exception_handle
AFE_EXCEPTION_BP:
    push 0x0
    push 3
    jmp exception_handle
AFE_EXCEPTION_OF:
    push 0x0
    push 4
    jmp exception_handle
AFE_EXCEPTION_BR:
    push 0x0
    push 5
    jmp exception_handle
AFE_EXCEPTION_UD:
    push 0x0
    push 6
    jmp exception_handle
AFE_EXCEPTION_NM:
    push 0x0
    push 7
    jmp exception_handle
AFE_EXCEPTION_DF:
    ;push 0x0
    push 8
    jmp exception_handle
AFE_EXCEPTION_MF:
    push 0x0
    push 9
    jmp exception_handle
AFE_EXCEPTION_TS:
    ;push 0x0
    push 10
    jmp exception_handle
AFE_EXCEPTION_NP:
    ;push 0x0
    push 11
    jmp exception_handle
AFE_EXCEPTION_SS:
    ;push 0x0
    push 12
    jmp exception_handle
AFE_EXCEPTION_GP:
    ;push 0xffffffff
    push 13
    jmp exception_handle
AFE_EXCEPTION_PF:
    ;push 0xffffffff
    push 14
    jmp exception_handle
AFE_EXCEPTION_RSVD:
    push 0x0
    push 15
    jmp exception_handle
AFE_EXCEPTION_MF2:
    push 0x0
    push 16
    jmp exception_handle
AFE_EXCEPTION_AC:
    push 0x0
    push 17
    jmp exception_handle
AFE_EXCEPTION_MC:
    push 0x0
    push 18
    jmp exception_handle
AFE_EXCEPTION_XM:
    push 0x0
    push 19
    jmp exception_handle

Save:
    save_regs
    mov esi,edx
    mov dx,ss
    mov ds,dx
    mov es,dx
    mov fs,dx
    mov edx,esi
    mov esi,esp
    inc dword[K_IntReenter]
    cmp dword[K_IntReenter],0
    jne Save_1
    mov esp,StackTop
    push Restart
    jmp [esi+48]

Save_1:
    push Restart_Reenter
    jmp [esi+48]


SYSCALL_GetTick:
    mov eax,0x00
    int 0x90
    ret



SYSCALL_ConWrite:
    mov eax,0x01
    mov ebx,[esp+4]
    mov ecx,[esp+8]
    ;xchg bx,bx
    int 0x90
    ret

SYSCALL_SendRec:
    mov eax,0x02
    mov ebx,[esp+4]
    mov ecx,[esp+8]
    mov edx,[esp+12]
    int 0x90
    ret


SYSCALL_ConWriteX:
    mov eax,0x03
    ;xchg bx,bx
    mov edx,[esp+4]
    int 0x90
    ret


SystemCall:
    call Save
    sti
    push esi
    push dword [ProcessReady]
    
    push edx
    push ecx
    push ebx
    call [syscall_table+eax*4]
    add esp,4*4

    pop esi
    mov [esi+44],eax
    cli
    ret

AFE_INT_0:
    interrupt_master 0

AFE_INT_1:
    interrupt_master 1
AFE_INT_2:
    interrupt_master 2
AFE_INT_3:
    interrupt_master 3
AFE_INT_4:
    interrupt_master 4
AFE_INT_5:
    interrupt_master 5
AFE_INT_6:
    interrupt_master 6
AFE_INT_7:
    interrupt_master 7
AFE_INT_8:
    interrupt_slave 8
AFE_INT_9:
    interrupt_slave 9
AFE_INT_10:
    interrupt_slave 10
AFE_INT_11:
    interrupt_slave 11
AFE_INT_12:
    interrupt_slave 12
AFE_INT_13:
    interrupt_slave 13
AFE_INT_14:
    interrupt_slave 14
AFE_INT_15:
    interrupt_slave 15
AFE_INT_16:
    interrupt_slave 16

exception_handle:
    call exception_handler
    add esp,8
    jmp $
    