bits 32

MAGIC_NUMBER equ 0x1BADB002
FLAGS        equ 0x0
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

section .multiboot
    align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

section .text
global _start
extern kernel_main

_start:
    cli                   ; Desabilita interrupções externas
    mov esp, stack_space  ; Aponta a pilha de memória
    call kernel_main      ; Pula para o Kernel em C
    hlt                   ; Trava a CPU se o kernel retornar

section .bss
resb 16384                ; Reserva 16KB para a Stack
stack_space:
