// ===== AuraOS 64-bit Kernel — анимация + меню + Booting =====

typedef unsigned short CHAR16;
typedef unsigned long long EFI_STATUS;
typedef unsigned int UINT32;
typedef unsigned long long UINTN;

// ===== Внешние переменные (из main.c) =====
extern UINT32 *g_fb;
extern UINT32 g_width;
extern UINT32 g_height;
extern UINT32 g_pitch;
extern void *g_conin;

// ===== EFI_INPUT_KEY =====
typedef struct {
    unsigned short ScanCode;
    CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

typedef struct {
    void *Reset;
    EFI_STATUS (*ReadKeyStroke)(void *This, EFI_INPUT_KEY *Key);
    void *WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL;

// ===== Графика =====
void put_pixel(UINT32 x, UINT32 y, UINT32 color) {
    if (x >= g_width || y >= g_height) return;
    g_fb[y * g_pitch + x] = color;
}

void fill_rect(UINT32 x, UINT32 y, UINT32 w, UINT32 h, UINT32 color) {
    for (UINT32 j = 0; j < h; j++) {
        for (UINT32 i = 0; i < w; i++) {
            put_pixel(x + i, y + j, color);
        }
    }
}

void clear_screen(UINT32 color) {
    for (UINT32 y = 0; y < g_height; y++) {
        for (UINT32 x = 0; x < g_width; x++) {
            g_fb[y * g_pitch + x] = color;
        }
    }
}

// ===== Шрифт 5x7 =====
const unsigned char font_A[7]  = {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
const unsigned char font_u[7]  = {0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0D};
const unsigned char font_r[7]  = {0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10};
const unsigned char font_a[7]  = {0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00};
const unsigned char font_B[7]  = {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E};
const unsigned char font_o[7]  = {0x00, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E};
const unsigned char font_t[7]  = {0x08, 0x08, 0x1C, 0x08, 0x08, 0x09, 0x06};
const unsigned char font_S[7]  = {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E};
const unsigned char font_f[7]  = {0x06, 0x09, 0x08, 0x1C, 0x08, 0x08, 0x08};
const unsigned char font_e[7]  = {0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x00};
const unsigned char font_M[7]  = {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11};
const unsigned char font_d[7]  = {0x01, 0x01, 0x0F, 0x11, 0x11, 0x0F, 0x00};
const unsigned char font_O[7]  = {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
const unsigned char font_i[7]  = {0x00, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E};
const unsigned char font_n[7]  = {0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11};
const unsigned char font_g[7]  = {0x00, 0x0E, 0x11, 0x11, 0x0F, 0x01, 0x0E};
const unsigned char font_space[7] = {0,0,0,0,0,0,0};

void draw_char(UINT32 x, UINT32 y, const unsigned char *font, UINT32 scale, UINT32 color) {
    for (UINT32 row = 0; row < 7; row++) {
        for (UINT32 col = 0; col < 5; col++) {
            if (font[row] & (1 << (4 - col))) {
                fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

void draw_logo(UINT32 x, UINT32 y, UINT32 scale, UINT32 color) {
    UINT32 step = 6 * scale;
    draw_char(x + 0 * step, y, font_A, scale, color);
    draw_char(x + 1 * step, y, font_u, scale, color);
    draw_char(x + 2 * step, y, font_r, scale, color);
    draw_char(x + 3 * step, y, font_a, scale, color);
    draw_char(x + 4 * step, y, font_space, scale, color);
    draw_char(x + 5 * step, y, font_B, scale, color);
    draw_char(x + 6 * step, y, font_o, scale, color);
    draw_char(x + 7 * step, y, font_o, scale, color);
    draw_char(x + 8 * step, y, font_t, scale, color);
}

// ===== Меню =====
int g_selected = 0;

void draw_menu(UINT32 scale) {
    UINT32 WHITE = 0x00FFFFFF;
    UINT32 BLACK = 0x00000000;
    UINT32 step = 6 * scale;
    UINT32 line_h = 10 * scale;
    UINT32 start_y = g_height / 2;
    UINT32 start_x = g_width / 3;
    
    // Очистка области меню
    fill_rect(start_x - 4*scale, start_y - 4*scale, 
              g_width/2 + 8*scale, line_h * 2 + 12*scale, BLACK);
    
    // ===== Пункт 1: AuraOS =====
    if (g_selected == 0) {
        fill_rect(start_x - 2*scale, start_y - 2*scale, 
                  g_width/2 + 4*scale, line_h, WHITE);
        draw_char(start_x, start_y, font_A, scale, BLACK);
        draw_char(start_x + step, start_y, font_u, scale, BLACK);
        draw_char(start_x + 2*step, start_y, font_r, scale, BLACK);
        draw_char(start_x + 3*step, start_y, font_a, scale, BLACK);
        draw_char(start_x + 4*step, start_y, font_O, scale, BLACK);
        draw_char(start_x + 5*step, start_y, font_S, scale, BLACK);
    } else {
        draw_char(start_x, start_y, font_A, scale, WHITE);
        draw_char(start_x + step, start_y, font_u, scale, WHITE);
        draw_char(start_x + 2*step, start_y, font_r, scale, WHITE);
        draw_char(start_x + 3*step, start_y, font_a, scale, WHITE);
        draw_char(start_x + 4*step, start_y, font_O, scale, WHITE);
        draw_char(start_x + 5*step, start_y, font_S, scale, WHITE);
    }
    
    // ===== Пункт 2: Safe Mode =====
    if (g_selected == 1) {
        fill_rect(start_x - 2*scale, start_y + line_h - 2*scale, 
                  g_width/2 + 4*scale, line_h, WHITE);
        draw_char(start_x, start_y + line_h, font_S, scale, BLACK);
        draw_char(start_x + step, start_y + line_h, font_a, scale, BLACK);
        draw_char(start_x + 2*step, start_y + line_h, font_f, scale, BLACK);
        draw_char(start_x + 3*step, start_y + line_h, font_e, scale, BLACK);
        draw_char(start_x + 4*step, start_y + line_h, font_space, scale, BLACK);
        draw_char(start_x + 5*step, start_y + line_h, font_M, scale, BLACK);
        draw_char(start_x + 6*step, start_y + line_h, font_o, scale, BLACK);
        draw_char(start_x + 7*step, start_y + line_h, font_d, scale, BLACK);
        draw_char(start_x + 8*step, start_y + line_h, font_e, scale, BLACK);
    } else {
        draw_char(start_x, start_y + line_h, font_S, scale, WHITE);
        draw_char(start_x + step, start_y + line_h, font_a, scale, WHITE);
        draw_char(start_x + 2*step, start_y + line_h, font_f, scale, WHITE);
        draw_char(start_x + 3*step, start_y + line_h, font_e, scale, WHITE);
        draw_char(start_x + 4*step, start_y + line_h, font_space, scale, WHITE);
        draw_char(start_x + 5*step, start_y + line_h, font_M, scale, WHITE);
        draw_char(start_x + 6*step, start_y + line_h, font_o, scale, WHITE);
        draw_char(start_x + 7*step, start_y + line_h, font_d, scale, WHITE);
        draw_char(start_x + 8*step, start_y + line_h, font_e, scale, WHITE);
    }
}

// ===== Экран загрузки =====
void draw_booting(void) {
    UINT32 WHITE = 0x00FFFFFF;
    UINT32 BLACK = 0x00000000;
    UINT32 scale = 4;
    UINT32 step = 6 * scale;
    
    clear_screen(BLACK);
    
    // Центр "Booting"
    UINT32 msg_w = 7 * step;
    UINT32 msg_x = (g_width - msg_w) / 2;
    UINT32 msg_y = g_height / 2 - 4 * scale;
    
    draw_char(msg_x + 0 * step, msg_y, font_B, scale, WHITE);
    draw_char(msg_x + 1 * step, msg_y, font_o, scale, WHITE);
    draw_char(msg_x + 2 * step, msg_y, font_o, scale, WHITE);
    draw_char(msg_x + 3 * step, msg_y, font_t, scale, WHITE);
    draw_char(msg_x + 4 * step, msg_y, font_i, scale, WHITE);
    draw_char(msg_x + 5 * step, msg_y, font_n, scale, WHITE);
    draw_char(msg_x + 6 * step, msg_y, font_g, scale, WHITE);
    
    // Три точки внизу
    UINT32 dot_y = msg_y + 9 * scale;
    UINT32 dot_x = (g_width - 5 * scale) / 2;
    fill_rect(dot_x + 0 * scale, dot_y, scale, scale, WHITE);
    fill_rect(dot_x + 2 * scale, dot_y, scale, scale, WHITE);
    fill_rect(dot_x + 4 * scale, dot_y, scale, scale, WHITE);
}

void delay(UINTN count) {
    for (volatile UINTN i = 0; i < count; i++);
}

// ===== Точка входа ядра =====
void kernel_main64(void) {
    UINT32 BLACK = 0x00000000;
    UINT32 WHITE = 0x00FFFFFF;
    UINT32 scale = 4;
    
    // Очистка
    clear_screen(BLACK);
    
    // Анимация логотипа (снизу вверх)
    UINT32 logo_w = 9 * 6 * scale;
    UINT32 start_x = (g_width - logo_w) / 2;
    
    for (UINT32 y = g_height; y > 150; y -= 8) {
        clear_screen(BLACK);
        draw_logo(start_x, y, scale, WHITE);
        delay(3000000);
    }
    
    // Логотип на месте
    clear_screen(BLACK);
    draw_logo(start_x, 150, scale, WHITE);
    
    // Меню
    draw_menu(scale);
    
    // ===== Ввод клавиш =====
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *conin = 
        (EFI_SIMPLE_TEXT_INPUT_PROTOCOL *)g_conin;
    
    while (1) {
        EFI_INPUT_KEY key;
        EFI_STATUS st = conin->ReadKeyStroke(conin, &key);
        
        if (st == 0) {
            // Стрелка вверх — ScanCode 0x01
            if (key.ScanCode == 0x01) {
                if (g_selected != 0) {
                    g_selected = 0;
                    draw_menu(scale);
                }
            }
            // Стрелка вниз — ScanCode 0x02
            else if (key.ScanCode == 0x02) {
                if (g_selected != 1) {
                    g_selected = 1;
                    draw_menu(scale);
                }
            }
            // Enter
            else if (key.UnicodeChar == 0x0D) {
                draw_booting();
                while (1) { __asm__ volatile ("hlt"); }
            }
        }
        
        delay(100000);
    }
}