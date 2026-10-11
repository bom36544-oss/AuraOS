// ===== AuraOS 64-bit Kernel =====
#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

void kernel_main64(void) {
    volatile char *video = (volatile char*) VGA_ADDRESS;
    const char *msg = "AuraOS 64-bit Kernel";
    int idx = (12 * VGA_WIDTH + 30) * 2;
    for (int i = 0; msg[i]; i++) {
        video[idx + i * 2] = msg[i];
        video[idx + i * 2 + 1] = 0x0A;
    }
    while (1);
}