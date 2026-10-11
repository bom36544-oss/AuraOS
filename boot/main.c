// ===== AuraOS UEFI Loader =====

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
    UINT32 MaxMode;
    UINT32 Mode;
    void *Info;
    UINTN SizeOfInfo;
    UINT64 FrameBufferBase;
    UINTN FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

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

// ===== memcpy / memset (компилятор иногда вставляет вызовы) =====
void *memcpy(void *dest, const void *src, unsigned long long n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    for (unsigned long long i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void *memset(void *dest, int c, unsigned long long n) {
    char *d = (char *)dest;
    for (unsigned long long i = 0; i < n; i++) {
        d[i] = (char)c;
    }
    return dest;
}

// ===== Глобальные переменные =====
UINT32 *g_fb = 0;
UINT32 g_width = 0;
UINT32 g_height = 0;
UINT32 g_pitch = 0;
void *g_conin = 0;

// ===== Мышь (пока в центре) =====
int g_mouse_x = 200;
int g_mouse_y = 200;

// ===== Форс линковки =====
extern void kernel_main(void);
extern void gui_main(void);
extern void gui_handle_up(void);
extern void gui_handle_down(void);
extern void gui_handle_enter(void);
extern void gui_handle_esc(void);

// ===== Точка входа UEFI =====
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
    (void)ImageHandle;
    
    char *BS = (char *)ST->BootServices;
    typedef EFI_STATUS (*LocateProtocol_t)(EFI_GUID*, void*, void**);
    LocateProtocol_t LocateProtocol = (LocateProtocol_t)*(void**)(BS + 0x140);
    
    // GOP
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = 0;
    EFI_STATUS status = LocateProtocol(&gop_guid, 0, (void**)&gop);
    
    if (status != 0 || gop == 0) {
        while (1) { __asm__ volatile ("hlt"); }
    }
    
    g_fb = (UINT32 *)(UINTN)gop->Mode->FrameBufferBase;
    
    typedef struct {
        UINT32 Version;
        UINT32 HorizontalResolution;
        UINT32 VerticalResolution;
        UINT32 PixelFormat;
        UINT32 PixelInformation[4];
        UINT32 PixelsPerScanLine;
    } GOP_MODE_INFO;
    
    GOP_MODE_INFO *info = (GOP_MODE_INFO *)gop->Mode->Info;
    g_width  = info->HorizontalResolution;
    g_height = info->VerticalResolution;
    g_pitch  = info->PixelsPerScanLine;
    
    g_conin = ST->ConIn;
    
    // Форс линковки
    volatile void *force[] = {
        (void*)&kernel_main,
        (void*)&gui_main,
        (void*)&gui_handle_up,
        (void*)&gui_handle_down,
        (void*)&gui_handle_enter,
        (void*)&gui_handle_esc
    };
    (void)force;
    
    // Вызов ядра
    kernel_main();
    
    while (1) { __asm__ volatile ("hlt"); }
    
    return 0;
}