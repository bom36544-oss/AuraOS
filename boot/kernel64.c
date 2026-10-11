// ===== AuraOS 64-bit Kernel (UEFI) =====
typedef unsigned short CHAR16;
typedef unsigned long long EFI_STATUS;
typedef void *EFI_HANDLE;

// ConOut protocol (передаётся из UEFI)
typedef struct {
    void *Reset;
    EFI_STATUS (*OutputString)(void *This, CHAR16 *String);
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// Указатель на ConOut (устанавливается из main.c)
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *g_conout = 0;

// Функция вывода
void kprint(CHAR16 *str) {
    if (g_conout) {
        g_conout->OutputString(g_conout, str);
    }
}

// Точка входа ядра — вызывается из main.c
void kernel_main64(void) {
    kprint(L"\r\n");
    kprint(L"=== AuraOS Kernel 64-bit ===\r\n");
    kprint(L"Kernel started!\r\n");
    kprint(L"\r\n");
    kprint(L"Welcome to AuraOS!\r\n");
    kprint(L"\r\n");
    kprint(L"Press any key...\r\n");
    
    // Бесконечный цикл
    while (1) {
        __asm__ volatile ("hlt");
    }
}