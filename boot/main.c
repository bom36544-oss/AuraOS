// ===== AuraOS UEFI GOP Test =====

typedef unsigned short CHAR16;
typedef unsigned long long EFI_STATUS;
typedef void *EFI_HANDLE;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned long long UINTN;

typedef struct {
    UINT32 Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} EFI_GUID;

typedef struct {
    char Hdr[24];
    CHAR16 *FirmwareVendor;
    UINT32 FirmwareRevision;
    char _pad1[4];
    EFI_HANDLE ConsoleInHandle;
    void *ConIn;
    EFI_HANDLE ConsoleOutHandle;
    void *ConOut;
    EFI_HANDLE StandardErrorHandle;
    void *StdErr;
    void *RuntimeServices;
    void *BootServices;
} EFI_SYSTEM_TABLE;

typedef struct {
    void *Reset;
    EFI_STATUS (*OutputString)(void *This, CHAR16 *String);
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE
typedef struct {
    UINT32 MaxMode;
    UINT32 Mode;
    void *Info;
    UINTN SizeOfInfo;
    UINT64 FrameBufferBase;
    UINTN FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

// EFI_GRAPHICS_OUTPUT_PROTOCOL
typedef struct {
    void *QueryMode;
    void *SetMode;
    void *Blt;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

static EFI_GUID gop_guid = {
    0x9042A9DE, 0x23DC, 0x4A38,
    {0x96, 0xFB, 0x7A, 0xDE, 0xD0, 0x80, 0x51, 0x6A}
};

void print(EFI_SYSTEM_TABLE *ST, CHAR16 *str) {
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *conout = 
        (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *)ST->ConOut;
    conout->OutputString(conout, str);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
    (void)ImageHandle;
    
    print(ST, L"AuraOS UEFI GOP Test\r\n");
    print(ST, L"Locating GOP...\r\n");
    
    // BootServices — это указатель на структуру
    char *BS = (char *)ST->BootServices;
    
    // Смещение LocateProtocol = 24 (Hdr) + 37*8 = 24 + 296 = 320 = 0x140
    // Всего функций до LocateProtocol: 37 (указателей)
    // Hdr = 24 байта
    // Функции:
    // 0x18 RaiseTPL
    // 0x20 RestoreTPL
    // 0x28 AllocatePages
    // 0x30 FreePages
    // 0x38 GetMemoryMap
    // 0x40 AllocatePool
    // 0x48 FreePool
    // 0x50 CreateEvent
    // 0x58 SetTimer
    // 0x60 WaitForEvent
    // 0x68 SignalEvent
    // 0x70 CloseEvent
    // 0x78 CheckEvent
    // 0x80 InstallProtocolInterface
    // 0x88 ReinstallProtocolInterface
    // 0x90 UninstallProtocolInterface
    // 0x98 HandleProtocol
    // 0xA0 Reserved
    // 0xA8 RegisterProtocolNotify
    // 0xB0 LocateHandle
    // 0xB8 LocateDevicePath
    // 0xC0 InstallConfigurationTable
    // 0xC8 LoadImage
    // 0xD0 StartImage
    // 0xD8 Exit
    // 0xE0 UnloadImage
    // 0xE8 ExitBootServices
    // 0xF0 GetNextMonotonicCount
    // 0xF8 Stall
    // 0x100 SetWatchdogTimer
    // 0x108 ConnectController
    // 0x110 DisconnectController
    // 0x118 OpenProtocol
    // 0x120 CloseProtocol
    // 0x128 OpenProtocolInformation
    // 0x130 ProtocolsPerHandle
    // 0x138 LocateHandleBuffer
    // 0x140 LocateProtocol ← здесь!
    
    typedef EFI_STATUS (*LocateProtocol_t)(EFI_GUID*, void*, void**);
    LocateProtocol_t LocateProtocol = (LocateProtocol_t)*(void**)(BS + 0x140);
    
    print(ST, L"Calling LocateProtocol...\r\n");
    
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = 0;
    EFI_STATUS status = LocateProtocol(&gop_guid, 0, (void**)&gop);
    
    print(ST, L"Returned from LocateProtocol\r\n");
    
    if (status != 0 || gop == 0) {
        print(ST, L"GOP not found!\r\n");
        while (1) { __asm__ volatile ("hlt"); }
    }
    
    print(ST, L"GOP found!\r\n");
    
    // Framebuffer
    UINT64 fb_base = gop->Mode->FrameBufferBase;
    UINT32 *fb = (UINT32 *)(UINTN)fb_base;
    
    // Залить 640x480 ярко-фиолетовым
    for (UINTN i = 0; i < 640 * 480; i++) {
        fb[i] = 0x00FF00FF;
    }
    
    print(ST, L"Screen filled!\r\n");
    
    while (1) { __asm__ volatile ("hlt"); }
    
    return 0;
}