global __idt_set
global __idt_default_exception_handler
global __idt_default_interrupt_handler
global __idt_exception_handlers
global __idt_spurious_interrupt_handler
global __idt_pit_interrupt_handler

section .text
__idt_default_exception_handler:
    jmp $

__idt_default_interrupt_handler:
    iretd

__idt_pit_interrupt_handler:
    pushad
    cld
    extern __pit_tick
    call __pit_tick
    popad
    iretd

%macro exception_handler 1
exception%1_handler:
    cli
    push byte 0
    push byte %1
    jmp except_handle
%endmacro

%macro exception_handler_with_error_code 1
exception%1_handler:
    cli
    push byte %1
    jmp except_handle
%endmacro

exception_handler 0
exception_handler 1
exception_handler 2
exception_handler 3
exception_handler 4
exception_handler 5
exception_handler 6
exception_handler 7
exception_handler_with_error_code 8
exception_handler 9
exception_handler_with_error_code 10
exception_handler_with_error_code 11
exception_handler_with_error_code 12
exception_handler_with_error_code 13
exception_handler_with_error_code 14
exception_handler 15
exception_handler 16
exception_handler_with_error_code 17
exception_handler 18
exception_handler 19

__idt_exception_handlers:
        dd exception0_handler
        dd exception1_handler
        dd exception2_handler
        dd exception3_handler
        dd exception4_handler
        dd exception5_handler
        dd exception6_handler
        dd exception7_handler
        dd exception8_handler
        dd exception9_handler
        dd exception10_handler
        dd exception11_handler
        dd exception12_handler
        dd exception13_handler
        dd exception14_handler
        dd exception15_handler
        dd exception16_handler
        dd exception17_handler
        dd exception18_handler
        dd exception19_handler

except_handle:
    push edi
    push esi
    push ebp
    push ebx
    push edx
    push ecx
    push eax
    push ss
    push cs

    extern handle_exception
    call handle_exception

    jmp $

__idt_spurious_interrupt_handler:
    iretd