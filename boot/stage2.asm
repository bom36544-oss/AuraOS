[bits 16]
[org 0x7E00]

stage2:
    cli

    mov ah, 0x0E
    mov al, '2'
    int 0x10

    lgdt [gdt_descriptor]

    mov ah, 0x0E
    mov al, 'G'
    int 0x10

    ; Включаем protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Far jump в protected mode
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

    ; Вывод "P" в VGA (НЕ BIOS!)
    mov byte [0xB8000], 'P'
    mov byte [0xB8001], 0x1F

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