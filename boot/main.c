// ===== AuraOS UEFI Loader (without gnu-efi) =====

typedef unsigned short CHAR16;
typedef unsigned long long EFI_STATUS;
typedef void *EFI_HANDLE;

// EFI_SYSTEM_TABLE — точная структура (x86_64 UEFI)
typedef struct {
    char Hdr[24];                    // 0x00: EFI_TABLE_HEADER
    CHAR16 *FirmwareVendor;          // 0x18
    unsigned int FirmwareRevision;   // 0x20
    char _pad1[4];                   // 0x24
    EFI_HANDLE ConsoleInHandle;      // 0x28
    void *ConIn;                     // 0x30
    EFI_HANDLE ConsoleOutHandle;     // 0x38
    void *ConOut;                    // 0x40
} EFI_SYSTEM_TABLE;

// EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
typedef struct {
    void *Reset;                     // 0x00
    EFI_STATUS (*OutputString)(void *This, CHAR16 *String);  // 0x08
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

void print(EFI_SYSTEM_TABLE *ST, CHAR16 *str) {
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *conout = (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *)ST->ConOut;
    conout->OutputString(conout, str);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
    (void)ImageHandle;

    print(ST, L"AuraOS UEFI Loader\r\n");
    print(ST, L"Hello, UEFI!\r\n");
    print(ST, L"\r\n");
    print(ST, L"Press any key...\r\n");

    while (1) {
        __asm__ volatile ("hlt");
    }

    return 0;
}