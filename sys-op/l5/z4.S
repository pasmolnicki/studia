; nasm -f elf z4.S -o z4.o
; gcc -m32 -no-pie -z execstack -o z4 z4.o

SECTION .data
prompt db 'Enter string numbers', 0Ah ; string with newline
prompt_len equ $ - prompt
input times 255 db 0                 ; buffer
input_len equ $ - input
sum_fmt db 'Sum: %d', 0Ah, 0        ; format string for printf


SECTION .text
global main
extern printf

main:
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
    je done_adding

    dec ecx             ; decrement bytes_read
    movzx eax, byte [esi]  ; load byte from input
    cmp eax, 0Ah           ; check for newline
    je done_adding

    sub eax, '0'          ; convert ASCII to integer
    add ebx, eax          ; sum += digit
    inc esi               ; move to next byte
    jmp loop_add

done_adding:
    ; printf("Sum: %d\n", sum)
    mov esi, ebx       ; move sum to esi for printf
    push esi           ; push sum
    push sum_fmt       ; push format string
    call printf
    add esp, 8         ; clean up stack (2 pushes)

    ; exit(0)
    mov eax, 1          ; syscall: exit
    mov ebx, 0          ; status: 0
    int 0x80