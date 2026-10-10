[bits 16]
[org 0x7E00]

stage2:
    cli

    mov ah, 0x0E
    mov al, '2'
    int 0x10

    ; ===== Читаем stage3 (сектор 3, 1 сектор) на 0x8000 =====
    mov ah, 0x02
    mov al, 1          ; ← 1 сектор
    mov ch, 0
    mov cl, 3
    mov dh, 0
    mov bx, 0x8000
    int 0x13

    ; ===== Читаем kernel64 (сектор 4, 1 сектор) на 0x1000 =====
    mov ah, 0x02
    mov al, 1          ; ← 1 сектор (было 100)
    mov ch, 0
    mov cl, 4
    mov dh, 0
    mov bx, 0x1000     ; ← 0x1000 (не 0x10000!)
    int 0x13

    mov ah, 0x0E
    mov al, 'K'
    int 0x10

    lgdt [gdt_descriptor]

    mov ah, 0x0E
    mov al, 'G'
    int 0x10

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

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

    call 0x8000

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