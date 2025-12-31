; nasm -f elf z5.S -o z5.o
; ld -m elf_i386 -o z5 z5.o
%include 'utils.S'

SECTION .data
prompt db 'Enter number: ', 0h, 0 ; string with newline
usage_prompt db 'Usage: <exec> <n1> <n2> ... <n9>', 0Ah, 0
error_prompt db 'Invalid number of arguments: ', 0h, 0
sum_txt     db 'Suma elementów: ', 0h, 0
diagonal_txt  db 'Suma przekątnej: ', 0h, 0

SECTION .bss
input resb 255         ; reserve 255 bytes for input
matrix resd 9          ; reserve space for 9 ints (3x3 matrix)

SECTION .text
global _start


_start:
    pop ebx ; int argc

    cmp ebx, 10
    jne error

    pop     ecx ; exec name
    xor     ecx, ecx ; counter = 0
    mov     esi, matrix ; int* p = matrix

.mainLoop:

    ; int num = load_number()
    pop     eax ; get the arg

    call    atoi ; convert to string

    mov     dword [esi], eax ; *p = num
    add     esi, 4 ; (4 = sizeof(int)) p++
    inc     ecx ; counter++

    cmp     ecx, 9
    jne     .mainLoop
    
.results:
    call    sum
    call    diagonal_sum
    call    quit

sum:
    ; save the registers
    push    ebx
    push    ecx

    mov     eax, matrix ; int* p = matrix
    xor     ebx, ebx ; int counter = 0
    xor     ecx, ecx ; int sum = 0

    .sum_loop:
        add     ecx, dword[eax] ; sum += *p
        add     eax, 4 ; p++
        inc     ebx ; counter++

        cmp     ebx, 9 ; if (counter != 9) continue
        jne     .sum_loop ; else break
    
    ; print the sum
    mov     eax, sum_txt
    call    sprint
    
    mov     eax, ecx
    call    iprintLF

    pop     ecx
    pop     ebx
    ret

diagonal_sum:
    push    ebx
    push    ecx

    mov     eax, matrix
    xor     ebx, ebx
    xor     ecx, ecx

    .sum_loop:
        add     ecx, dword[eax] ; sum += *p
        add     eax, 16 ; 4 * sizeof(int) p += 4
        inc     ebx
        cmp     ebx, 3
        jne     .sum_loop
    
    mov     eax, diagonal_txt
    call    sprint

    mov     eax, ecx
    call    iprintLF

    pop     ecx
    pop     ebx
    ret

error:
    ; Invalid number of arguments: <eax>\n
    push    eax
    mov     eax, error_prompt
    call    sprint
    pop     eax
    call    iprintLF

    ; Usage
    mov     eax, usage_prompt
    call    sprint
    call    quit
    