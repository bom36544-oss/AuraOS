#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

// ===== Прототипы =====
unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char value);
void hide_cursor();
unsigned char keyboard_read();
void delay(int count);
void clear_screen(volatile char *video);
void draw_logo_at(volatile char *video, int y, unsigned char color);
void redraw_menu(volatile char *video, int selected);

// ============================================
// kernel_main — ПЕРВАЯ функция!
// ============================================
void kernel_main(void) {
    volatile char *video = (volatile char*) VGA_ADDRESS;

    hide_cursor();
    clear_screen(video);

    // ===== АНИМАЦИЯ: логотип снизу вверх =====
    for (int y = VGA_HEIGHT - 1; y >= 2; y--) {
        clear_screen(video);
        draw_logo_at(video, y, 0x0D);  // ярко-фиолетовый
        delay(8000000);
    }

    // Пауза
    delay(10000000);
    clear_screen(video);
    draw_logo_at(video, 2, 0x0D);  // на месте

    // Меню (без линии)
    redraw_menu(video, 0);

    // ===== Цикл клавиатуры =====
    int selected = 0;
    while (1) {
        unsigned char sc = keyboard_read();

        if (sc == 0x48) {        // стрелка вверх
            if (selected != 0) {
                selected = 0;
                redraw_menu(video, selected);
            }
        } else if (sc == 0x50) { // стрелка вниз
            if (selected != 1) {
                selected = 1;
                redraw_menu(video, selected);
            }
        } else if (sc == 0x1C) { // Enter
            clear_screen(video);
            const char *msg = (selected == 0) ? "Booting AuraOS..." : "Safe Mode activated";
            int len = (selected == 0) ? 18 : 19;
            int x = (VGA_WIDTH - len) / 2;
            int y = VGA_HEIGHT / 2;
            int idx = (y * VGA_WIDTH + x) * 2;
            unsigned char color = (selected == 0) ? 0x0A : 0x0E;
            for (int i = 0; i < len; i++) {
                video[idx + i * 2] = msg[i];
                video[idx + i * 2 + 1] = color;
            }
            while (1);
        }
    }
}

// ============================================
// Вспомогательные функции
// ============================================

unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void hide_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

unsigned char keyboard_read() {
    while (!(inb(0x64) & 1));
    return inb(0x60);
}

void delay(int count) {
    for (volatile int i = 0; i < count; i++);
}

void clear_screen(volatile char *video) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0x0F;
    }
}

void draw_logo_at(volatile char *video, int y, unsigned char color) {
    int x = (VGA_WIDTH - 9) / 2;
    int ti = (y * VGA_WIDTH + x) * 2;
    video[ti + 0]  = 'A';  video[ti + 1]  = color;
    video[ti + 2]  = 'u';  video[ti + 3]  = color;
    video[ti + 4]  = 'r';  video[ti + 5]  = color;
    video[ti + 6]  = 'a';  video[ti + 7]  = color;
    video[ti + 8]  = ' ';  video[ti + 9]  = color;
    video[ti + 10] = 'B';  video[ti + 11] = color;
    video[ti + 12] = 'o';  video[ti + 13] = color;
    video[ti + 14] = 'o';  video[ti + 15] = color;
    video[ti + 16] = 't';  video[ti + 17] = color;
}

// ===== Меню (без линии) =====
void redraw_menu(volatile char *video, int selected) {
    int menu_y = 11;

    // Очистка области меню
    for (int y = 10; y <= 14; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            video[(y * VGA_WIDTH + x) * 2] = ' ';
            video[(y * VGA_WIDTH + x) * 2 + 1] = 0x0F;
        }
    }

    // ===== Пункт 1: "> AuraOS <" =====
    int item1_x = (VGA_WIDTH - 11) / 2;
    unsigned char c1 = (selected == 0) ? 0x1F : 0x07;
    int mi = (menu_y * VGA_WIDTH + item1_x) * 2;
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

    // ===== Пункт 2: "> Safe Mode <" =====
    int item2_x = (VGA_WIDTH - 13) / 2;
    unsigned char c2 = (selected == 1) ? 0x1F : 0x07;
    int si = ((menu_y + 2) * VGA_WIDTH + item2_x) * 2;
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