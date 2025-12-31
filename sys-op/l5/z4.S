; nasm -f elf z4.S -o z4.o
; ld -m elf_i386 -o z4 z4.o

%include 'utils.S'

SECTION .data
prompt db 'Enter number: ', 0h, 0 ; string with newline
prompt_len equ $ - prompt
input times 255 db 0                 ; buffer
input_len equ $ - input
sum_txt db 'Sum of digits: ', 0h, 0

SECTION .text
global _start

_start:
    ; write(stdout, prompt, 13)
    mov eax, 4          ; syscall: write
    mov ebx, 1          ; file descriptor: stdout
    mov ecx, prompt     ; buffer
    mov edx, prompt_len         ; length
    int 0x80 
    
    ; read(stdin, input, 255)
    mov eax, 3          ; syscall: read
    mov ebx, 0          ; file descriptor: stdin
    mov ecx, input      ; buffer
    mov edx, 255        ; length
    int 0x80


    mov esi, input      ; pointer to input buffer
    mov ecx, eax        ; bytes_read = eax
    mov ebx, 0          ; sum = 0

loop_add:
    ; char* input;
    ; int bytes_read = eax;
    ; int sum = 0;

    ; while(true) {
    ;
        ; if (ecx == 0 {
        ;   break;
        ; }
        ; bytes_read--;
        ; byte ch = input[0];
        ; if (ch == '\n') {
        ;   break;
        ; }
        ; int digit = ch - '0';
        ; sum += digit;
        ; input++;
    ; }
    ; printf("Sum: %d\n", sum);

    cmp ecx, 0
    je .done_adding

    dec ecx             ; decrement bytes_read
    movzx eax, byte [esi]  ; load byte from input
    cmp eax, 0Ah           ; check for newline
    je .done_adding

    sub eax, '0'          ; convert ASCII to integer
    add ebx, eax          ; sum += digit
    inc esi               ; move to next byte
    jmp loop_add

.done_adding:
    mov eax, sum_txt
    call sprint           ; print "Sum of digits: "
    
    mov eax, ebx          ; move sum into eax for printf
    call iprintLF         ; print sum with linefeed

    ; exit(0)
    call quit
