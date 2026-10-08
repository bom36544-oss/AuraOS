[bits 16]
[org 0x7E00]

stage2:
    cli

    ; Вывод "2"
    mov ah, 0x0E
    mov al, '2'
    int 0x10

    ; Загружаем ядро (сектор 3, 100 секторов) на 0x1000
    mov ah, 0x02
    mov al, 100
    mov ch, 0
    mov cl, 3
    mov dh, 0
    mov bx, 0x1000
    int 0x13

    ; Вывод "K"
    mov ah, 0x0E
    mov al, 'K'
    int 0x10

    lgdt [gdt_descriptor]

    ; Вывод "G"
    mov ah, 0x0E
    mov al, 'G'
    int 0x10

    ; Включаем protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Переход
    jmp 0x08:protected_mode

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    ; Вызов ядра на C (kernel_main — первый в .text)
    call 0x1000

    hlt
    jmp $

gdt_start:
    dq 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start