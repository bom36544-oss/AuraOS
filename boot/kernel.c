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
void draw_big_char(volatile char *video, int x, int y, char c, unsigned char color);
void draw_logo_big(volatile char *video, int y, unsigned char color);
void draw_box(volatile char *video, int x, int y, int w, int h, unsigned char color);
void draw_menu_item(volatile char *video, int y, const char *text, int selected);
void redraw_menu(volatile char *video, int selected);

// ============================================
// kernel_main — ПЕРВАЯ функция!
// ============================================
void kernel_main(void) {
    volatile char *video = (volatile char*) VGA_ADDRESS;

    hide_cursor();
    clear_screen(video);

    // ===== АНИМАЦИЯ: большой логотип снизу вверх =====
    for (int y = VGA_HEIGHT - 2; y >= 2; y--) {
        clear_screen(video);
        draw_logo_big(video, y, 0x0D);  // ярко-фиолетовый
        delay(8000000);
    }

    // Пауза
    delay(10000000);
    clear_screen(video);
    draw_logo_big(video, 2, 0x0D);

    // Меню
    redraw_menu(video, 0);

    // ===== Цикл клавиатуры =====
    int selected = 0;
    while (1) {
        unsigned char sc = keyboard_read();

        if (sc == 0x48) {        // вверх
            if (selected != 0) {
                selected = 0;
                redraw_menu(video, selected);
            }
        } else if (sc == 0x50) { // вниз
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
// Вспомогательные
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

// ===== Большой символ (2×2 знакоместа) =====
void draw_big_char(volatile char *video, int x, int y, char c, unsigned char color) {
    int p1 = (y * VGA_WIDTH + x) * 2;
    video[p1] = c;     video[p1 + 1] = color;
    int p2 = (y * VGA_WIDTH + x + 1) * 2;
    video[p2] = c;     video[p2 + 1] = color;
    int p3 = ((y + 1) * VGA_WIDTH + x) * 2;
    video[p3] = c;     video[p3 + 1] = color;
    int p4 = ((y + 1) * VGA_WIDTH + x + 1) * 2;
    video[p4] = c;     video[p4 + 1] = color;
}

// ===== Большой логотип "Aura Boot" =====
void draw_logo_big(volatile char *video, int y, unsigned char color) {
    // "Aura Boot" — 9 символов × 2 = 18
    int total_w = 9 * 2;
    int x = (VGA_WIDTH - total_w) / 2;

    draw_big_char(video, x + 0,  y, 'A', color);
    draw_big_char(video, x + 2,  y, 'u', color);
    draw_big_char(video, x + 4,  y, 'r', color);
    draw_big_char(video, x + 6,  y, 'a', color);
    draw_big_char(video, x + 8,  y, ' ', color);
    draw_big_char(video, x + 10, y, 'B', color);
    draw_big_char(video, x + 12, y, 'o', color);
    draw_big_char(video, x + 14, y, 'o', color);
    draw_big_char(video, x + 16, y, 't', color);
}

// ===== Рамка вокруг меню =====
void draw_box(volatile char *video, int x, int y, int w, int h, unsigned char color) {
    // Верхняя граница
    video[(y * VGA_WIDTH + x) * 2] = '+';
    video[(y * VGA_WIDTH + x) * 2 + 1] = color;
    for (int i = 1; i < w - 1; i++) {
        video[(y * VGA_WIDTH + x + i) * 2] = '-';
        video[(y * VGA_WIDTH + x + i) * 2 + 1] = color;
    }
    video[(y * VGA_WIDTH + x + w - 1) * 2] = '+';
    video[(y * VGA_WIDTH + x + w - 1) * 2 + 1] = color;

    // Нижняя граница
    video[((y + h - 1) * VGA_WIDTH + x) * 2] = '+';
    video[((y + h - 1) * VGA_WIDTH + x) * 2 + 1] = color;
    for (int i = 1; i < w - 1; i++) {
        video[((y + h - 1) * VGA_WIDTH + x + i) * 2] = '-';
        video[((y + h - 1) * VGA_WIDTH + x + i) * 2 + 1] = color;
    }
    video[((y + h - 1) * VGA_WIDTH + x + w - 1) * 2] = '+';
    video[((y + h - 1) * VGA_WIDTH + x + w - 1) * 2 + 1] = color;

    // Боковые границы
    for (int j = 1; j < h - 1; j++) {
        video[((y + j) * VGA_WIDTH + x) * 2] = '|';
        video[((y + j) * VGA_WIDTH + x) * 2 + 1] = color;
        video[((y + j) * VGA_WIDTH + x + w - 1) * 2] = '|';
        video[((y + j) * VGA_WIDTH + x + w - 1) * 2 + 1] = color;
    }
}

// ===== Пункт меню =====
void draw_menu_item(volatile char *video, int y, const char *text, int selected) {
    int x = (VGA_WIDTH - 11) / 2;
    unsigned char c = selected ? 0x1F : 0x07;

    // Подсветка выбранного — закрашиваем всю строку
    if (selected) {
        for (int i = 0; i < 11; i++) {
            video[(y * VGA_WIDTH + x + i) * 2] = ' ';
            video[(y * VGA_WIDTH + x + i) * 2 + 1] = c;
        }
    }

    // Текст
    int idx = (y * VGA_WIDTH + x) * 2;
    for (int i = 0; text[i]; i++) {
        video[idx + i * 2] = text[i];
        video[idx + i * 2 + 1] = c;
    }
}

// ===== Меню с рамкой =====
void redraw_menu(volatile char *video, int selected) {
    // Очистка области меню
    for (int y = 9; y <= 17; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            video[(y * VGA_WIDTH + x) * 2] = ' ';
            video[(y * VGA_WIDTH + x) * 2 + 1] = 0x0F;
        }
    }

    // ===== Рамка вокруг меню =====
    int box_x = (VGA_WIDTH - 30) / 2;
    int box_y = 10;
    int box_w = 30;
    int box_h = 6;
    draw_box(video, box_x, box_y, box_w, box_h, 0x0B);  // голубая рамка

    // ===== Пункт 1: AuraOS =====
    draw_menu_item(video, box_y + 1, "   > AuraOS < ", selected == 0 ? 1 : 0);

    // ===== Пункт 2: Safe Mode =====
    draw_menu_item(video, box_y + 3, " > Safe Mode < ", selected == 1 ? 1 : 0);
}