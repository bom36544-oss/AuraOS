[bits 16]
[org 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Вывод "B"
    mov ah, 0x0E
    mov al, 'B'
    int 0x10

    ; Читаем stage2 (сектор 2, 64 сектора) на 0x7E00
    mov ah, 0x02
    mov al, 64
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov bx, 0x7E00
    int 0x13

    ; Вывод "S"
    mov ah, 0x0E
    mov al, 'S'
    int 0x10

    jmp 0x7E00

times 510-($-$$) db 0
dw 0xAA55