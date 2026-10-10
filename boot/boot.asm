[bits 16]
[org 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov [boot_drive], dl

    mov ah, 0x0E
    mov al, 'B'
    int 0x10

    ; ===== Читаем stage2 (1 сектор) =====
    mov dl, [boot_drive]
    mov ah, 0x02
    mov al, 1          ; ← было 64
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov bx, 0x7E00
    int 0x13

    jc disk_error

    mov ah, 0x0E
    mov al, 'S'
    int 0x10

    jmp 0x7E00

disk_error:
    mov ah, 0x0E
    mov al, 'E'
    int 0x10
    hlt
    jmp $

boot_drive: db 0

times 510-($-$$) db 0
dw 0xAA55