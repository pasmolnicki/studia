%include 'utils.S'

SECTION .data
usage_prompt db 'Usage: <exec> <n1>', 0Ah, 0
error_prompt db 'Invalid number of arguments: ', 0h, 0
newline     db 0Ah, 0h

SECTION .text
global _start


_start:
    pop     eax
    cmp     eax, 2
    jne     error_usage

    pop     eax
    pop     eax ; actual number

    call    atoi
    call    print_hex
    call    quit

print_hex:
    push    eax
    push    ebx
    push    ecx
    push    edx

    mov     ecx, 0  ; counter of chars

.hex_loop:
    mov     edx, eax
    and     edx, 0Fh ; get last 4 bits
    cmp     edx, 9
    jg      .is_letter
    add     edx, '0' ; convert 0-9 to ascii
    jmp     .push_char

.is_letter:
    add     edx, 55 ; convert to letter ('A' - 10)

.push_char:
    push    edx ; push char to stack
    inc     ecx ; counter++
    shr     eax, 4 ; shift right by 4 bits
    cmp     eax, 0
    jnz     .hex_loop

.print_loop:
    mov     eax, esp ; point to char on stack (sprint uses slen which sees the byte + nulls)
    cmp     ecx, 1
    call    sprint
    pop     eax ; remove char from stack
    dec     ecx
    cmp     ecx, 0
    jnz     .print_loop

    mov     eax, newline
    call    sprint

    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
    ret


error_usage:
    push    eax
    mov     eax, error_prompt
    call    sprint

    pop     eax
    call    iprintLF

    mov     eax, usage_prompt
    call    sprint

    call quit
