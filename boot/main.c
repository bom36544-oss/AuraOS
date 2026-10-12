// ===== AuraOS UEFI Loader — с двойной буферизацией =====

typedef unsigned short CHAR16;
typedef unsigned long long EFI_STATUS;
typedef void *EFI_HANDLE;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned long long UINTN;
typedef signed long long INT64;

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

typedef struct {
    INT64 RelativeMovementX;
    INT64 RelativeMovementY;
    INT64 RelativeMovementZ;
    unsigned char LeftButton;
    unsigned char RightButton;
} EFI_SIMPLE_POINTER_STATE;

typedef struct {
    void *Reset;
    EFI_STATUS (*GetState)(void *This, EFI_SIMPLE_POINTER_STATE *State);
    void *WaitForInput;
} EFI_SIMPLE_POINTER_PROTOCOL;

static EFI_GUID gop_guid = {
    0x9042A9DE, 0x23DC, 0x4A38,
    {0x96, 0xFB, 0x7A, 0xDE, 0xD0, 0x80, 0x51, 0x6A}
};

static EFI_GUID pointer_guid = {
    0x31878C87, 0x0B75, 0x11D5,
    {0x9A, 0x4F, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}
};

// ===== memcpy / memset =====
void *memcpy(void *dest, const void *src, unsigned long long n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    for (unsigned long long i = 0; i < n; i++) d[i] = s[i];
    return dest;
}
void *memset(void *dest, int c, unsigned long long n) {
    char *d = (char *)dest;
    for (unsigned long long i = 0; i < n; i++) d[i] = (char)c;
    return dest;
}

// ===== Глобальные переменные =====
UINT32 *g_fb = 0;
UINT32 *g_back = 0;    // back buffer
UINT32 g_width = 0;
UINT32 g_height = 0;
UINT32 g_pitch = 0;
void *g_conin = 0;

int g_mouse_x = 200;
int g_mouse_y = 200;

EFI_SIMPLE_POINTER_PROTOCOL *g_mouse = 0;

// ===== НОВОЕ: состояние кнопки мыши =====
unsigned char g_mouse_left = 0;       // текущее состояние ЛКМ (0/1)
unsigned char g_mouse_left_prev = 0;  // состояние в прошлом кадре

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
    
    // AllocatePool (смещение 0x40 в BootServices)
    typedef EFI_STATUS (*AllocatePool_t)(int PoolType, UINTN Size, void **Buffer);
    AllocatePool_t AllocatePool = (AllocatePool_t)*(void**)(BS + 0x40);
    
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
    
    // ===== Выделить back buffer =====
    UINTN buffer_size = g_pitch * g_height * sizeof(UINT32);
    void *buffer = 0;
    status = AllocatePool(0 /* EfiLoaderData */, buffer_size, &buffer);
    
    if (status == 0 && buffer != 0) {
        g_back = (UINT32 *)buffer;
    } else {
        // Fallback — если не выделилось, использовать framebuffer напрямую
        g_back = g_fb;
    }
    
    g_conin = ST->ConIn;
    
    // Мышь
    EFI_SIMPLE_POINTER_PROTOCOL *mouse = 0;
    status = LocateProtocol(&pointer_guid, 0, (void**)&mouse);
    if (status == 0 && mouse != 0) {
        g_mouse = mouse;
        g_mouse_x = g_width / 2;
        g_mouse_y = g_height / 2;
    }
    
    // Форс линковки
    volatile void *force[] = {
        (void*)&kernel_main, (void*)&gui_main,
        (void*)&gui_handle_up, (void*)&gui_handle_down,
        (void*)&gui_handle_enter, (void*)&gui_handle_esc
    };
    (void)force;
    
    kernel_main();
    
    while (1) { __asm__ volatile ("hlt"); }
    
    return 0;
}