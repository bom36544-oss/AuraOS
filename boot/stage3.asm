[bits 32]
[org 0x8000]

stage3:
    ; Debug "3"
    mov byte [0xB8000 + 160], '3'
    mov byte [0xB8000 + 161], 0x0E

    ; ===== Очистка таблиц (16 КБ) =====
    mov edi, 0x100000
    xor eax, eax
    mov ecx, 4096
    rep stosd

    mov byte [0xB8000 + 162], 'T'
    mov byte [0xB8000 + 163], 0x0E

    ; ===== PML4[0] = 0x101003 =====
    mov dword [0x100000], 0x101003
    ; ===== PDP[0] = 0x102003 =====
    mov dword [0x101000], 0x102003
    ; ===== PD[0] = 0x103003 =====
    mov dword [0x102000], 0x103003

    mov byte [0xB8000 + 164], 'A'
    mov byte [0xB8000 + 165], 0x0E

    ; ===== PT: маппим первые 2 МБ =====
    mov edi, 0x103000
    mov eax, 0x00000003
    mov ecx, 512
map_pt:
    mov [edi], eax
    add eax, 0x1000
    add edi, 8
    loop map_pt

    mov byte [0xB8000 + 166], 'B'
    mov byte [0xB8000 + 167], 0x0E

    ; ===== CR3 =====
    mov eax, 0x100000
    mov cr3, eax

    mov byte [0xB8000 + 168], 'C'
    mov byte [0xB8000 + 169], 0x0E

    ; ===== PAE (CR4.PAE) =====
    mov eax, cr4
    or eax, (1 << 5)
    mov cr4, eax

    mov byte [0xB8000 + 170], 'D'
    mov byte [0xB8000 + 171], 0x0E

    ; ===== LME (EFER.LME) =====
    mov ecx, 0xC0000080
    rdmsr
    or eax, (1 << 8)
    wrmsr

    mov byte [0xB8000 + 172], 'E'
    mov byte [0xB8000 + 173], 0x0E

    ; ===== GDT 64 =====
    lgdt [gdt64_descriptor]

    mov byte [0xB8000 + 174], 'F'
    mov byte [0xB8000 + 175], 0x0E

    ; ===== Paging =====
    mov eax, cr0
    or eax, (1 << 31)
    mov cr0, eax

    ; ===== Far jump в 64-бит =====
    jmp 0x08:long_mode_entry

[bits 64]
long_mode_entry:
    ; Настроить сегменты
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; ===== Установить стек (rsp) =====
    mov rsp, 0x90000

    ; Debug "L64"
    mov rax, 0xB8000
    mov byte [rax + 176], 'L'
    mov byte [rax + 177], 0x0A
    mov byte [rax + 178], '6'
    mov byte [rax + 179], 0x0A
    mov byte [rax + 180], '4'
    mov byte [rax + 181], 0x0A

    ; ===== Вызов 64-бит ядра (0x1000) =====
    mov rax, 0x1000
    call rax

    ; Если вернётся
    hlt
    jmp $

; ===== GDT 64-бит =====
gdt64_start:
    dq 0x0
gdt64_code:
    dq 0x00AF9A000000FFFF
gdt64_data:
    dq 0x00CF92000000FFFF
gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64_start - 1
    dd gdt64_start