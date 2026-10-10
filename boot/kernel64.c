// ===== 64-bit minimal kernel =====
#define VGA_ADDRESS 0xB8000

void kernel_main64(void) {
    volatile char *video = (volatile char*) VGA_ADDRESS;

    // "K64" в VGA (жёлтый)
    video[182] = 'K';
    video[183] = 0x0E;
    video[184] = '6';
    video[185] = 0x0E;
    video[186] = '4';
    video[187] = 0x0E;

    while (1);
}