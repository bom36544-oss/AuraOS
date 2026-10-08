#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80

unsigned char keyboard_read();
void redraw_menu(volatile char *video, int selected);
void hide_cursor();
void outb(unsigned short port, unsigned char value);

void kernel_main(void) {
    volatile char *video = (volatile char*) VGA_ADDRESS;
    
    // Скрыть курсор
    hide_cursor();
    
    // Очистка
    for (int i = 0; i < VGA_WIDTH * 25 * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0x0F;
    }
    
    // "Aura Boot"
    int tx = (VGA_WIDTH - 9) / 2;
    int ty = 2;
    int ti = (ty * VGA_WIDTH + tx) * 2;
    video[ti + 0]  = 'A';  video[ti + 1]  = 0x0B;
    video[ti + 2]  = 'u';  video[ti + 3]  = 0x0B;
    video[ti + 4]  = 'r';  video[ti + 5]  = 0x0B;
    video[ti + 6]  = 'a';  video[ti + 7]  = 0x0B;
    video[ti + 8]  = ' ';  video[ti + 9]  = 0x0B;
    video[ti + 10] = 'B';  video[ti + 11] = 0x0B;
    video[ti + 12] = 'o';  video[ti + 13] = 0x0B;
    video[ti + 14] = 'o';  video[ti + 15] = 0x0B;
    video[ti + 16] = 't';  video[ti + 17] = 0x0B;
    
    // Линия
    int ly = 4;
    for (int i = 0; i < VGA_WIDTH; i++) {
        video[(ly * VGA_WIDTH + i) * 2] = '-';
        video[(ly * VGA_WIDTH + i) * 2 + 1] = 0x0B;
    }
    
    // Меню
    redraw_menu(video, 0);
    
    int selected = 0;
    while (1) {
        unsigned char sc = keyboard_read();
        if (sc == 0x48) { selected = 0; redraw_menu(video, selected); }
        else if (sc == 0x50) { selected = 1; redraw_menu(video, selected); }
    }
}

unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Скрыть курсор
void hide_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);  // бит 5 = отключить курсор
}

unsigned char keyboard_read() {
    while (!(inb(0x64) & 1));
    return inb(0x60);
}

void redraw_menu(volatile char *video, int selected) {
    for (int y = 10; y <= 15; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            video[(y * VGA_WIDTH + x) * 2] = ' ';
            video[(y * VGA_WIDTH + x) * 2 + 1] = 0x0F;
        }
    }
    
    unsigned char c1 = (selected == 0) ? 0x1F : 0x07;
    int mi = (11 * VGA_WIDTH + 34) * 2;
    video[mi + 0]  = '>';  video[mi + 1]  = c1;
    video[mi + 2]  = ' ';  video[mi + 3]  = c1;
    video[mi + 4]  = 'A';  video[mi + 5]  = c1;
    video[mi + 6]  = 'u';  video[mi + 7]  = c1;
    video[mi + 8]  = 'r';  video[mi + 9]  = c1;
    video[mi + 10] = 'a';  video[mi + 11] = c1;
    video[mi + 12] = 'O';  video[mi + 13] = c1;
    video[mi + 14] = 'S';  video[mi + 15] = c1;
    video[mi + 16] = ' ';  video[mi + 17] = c1;
    video[mi + 18] = '<';  video[mi + 19] = c1;
    
    unsigned char c2 = (selected == 1) ? 0x1F : 0x07;
    int si = (13 * VGA_WIDTH + 34) * 2;
    video[si + 0]  = '>';  video[si + 1]  = c2;
    video[si + 2]  = ' ';  video[si + 3]  = c2;
    video[si + 4]  = 'S';  video[si + 5]  = c2;
    video[si + 6]  = 'a';  video[si + 7]  = c2;
    video[si + 8]  = 'f';  video[si + 9]  = c2;
    video[si + 10] = 'e';  video[si + 11] = c2;
    video[si + 12] = ' ';  video[si + 13] = c2;
    video[si + 14] = 'M';  video[si + 15] = c2;
    video[si + 16] = 'o';  video[si + 17] = c2;
    video[si + 18] = 'd';  video[si + 19] = c2;
    video[si + 20] = 'e';  video[si + 21] = c2;
    video[si + 22] = ' ';  video[si + 23] = c2;
    video[si + 24] = '<';  video[si + 25] = c2;
}