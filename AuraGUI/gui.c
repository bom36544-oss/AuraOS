// ===== AuraGUI — рабочий стол + окна + курсор =====

#include "gui.h"
#include "graphics.h"

#define COLOR_BG       0x00202020
#define COLOR_PANEL    0x00404040
#define COLOR_ICON_BG  0x00606060
#define COLOR_ICON_SEL 0x00A0A0A0
#define COLOR_ICON_TXT 0x00FFFFFF
#define COLOR_WIN_BG   0x00E0E0E0
#define COLOR_WIN_BAR  0x00505050
#define COLOR_WIN_BRD  0x00FFFFFF

#define CURSOR_W 24
#define CURSOR_H 40

// ===== Состояние =====
static int g_desktop_selected = 0;
static int g_window_open = 0;

// ===== Буфер под курсором =====
static UINT32 g_cursor_buf[CURSOR_W * CURSOR_H];
static int g_cursor_saved = 0;
static int g_cursor_old_x = -1;
static int g_cursor_old_y = -1;

// ===== Иконка =====
static void draw_icon(UINT32 x, UINT32 y, const unsigned char *letter, int selected) {
    UINT32 size = 64;
    UINT32 bg = selected ? COLOR_ICON_SEL : COLOR_ICON_BG;
    
    gfx_fill_rect(x, y, size, size, bg);
    gfx_draw_rect_border(x, y, size, size, 2, COLOR_ICON_TXT);
    
    UINT32 char_scale = 6;
    UINT32 char_w = 5 * char_scale;
    UINT32 char_h = 7 * char_scale;
    gfx_draw_char(x + (size - char_w) / 2, y + (size - char_h) / 2, letter, char_scale, COLOR_ICON_TXT);
}

// ===== Нарисовать курсор (в back buffer) =====
void gui_draw_cursor(void) {
    gfx_draw_char(g_mouse_x, g_mouse_y, font_A, 4, COLOR_ICON_TXT);
    gfx_fill_rect(g_mouse_x + 8, g_mouse_y + 32, 8, 2, COLOR_ICON_TXT);
    gfx_fill_rect(g_mouse_x + 11, g_mouse_y + 29, 2, 8, COLOR_ICON_TXT);
}

// ===== Сохранить фон под курсором (ДО рисования курсора!) =====
void gui_save_under_cursor(void) {
    extern UINT32 *g_back;
    if (!g_back) return;
    
    int sx = g_mouse_x;
    int sy = g_mouse_y;
    
    for (int j = 0; j < CURSOR_H; j++) {
        for (int i = 0; i < CURSOR_W; i++) {
            int px = sx + i;
            int py = sy + j;
            UINT32 color = COLOR_BG;
            if (px >= 0 && py >= 0 && px < (int)g_width && py < (int)g_height) {
                color = g_back[py * g_pitch + px];
            }
            g_cursor_buf[j * CURSOR_W + i] = color;
        }
    }
    g_cursor_saved = 1;
    g_cursor_old_x = sx;
    g_cursor_old_y = sy;
}

// ===== Восстановить фон под старым курсором =====
void gui_restore_under_cursor(void) {
    if (!g_cursor_saved) return;
    
    extern UINT32 *g_back;
    if (!g_back) return;
    
    int sx = g_cursor_old_x;
    int sy = g_cursor_old_y;
    
    for (int j = 0; j < CURSOR_H; j++) {
        for (int i = 0; i < CURSOR_W; i++) {
            int px = sx + i;
            int py = sy + j;
            if (px >= 0 && py >= 0 && px < (int)g_width && py < (int)g_height) {
                g_back[py * g_pitch + px] = g_cursor_buf[j * CURSOR_W + i];
            }
        }
    }
    
    // Скопировать восстановленную область в fb (убрать старый курсор с экрана)
    gfx_present_rect(sx, sy, CURSOR_W, CURSOR_H);
    
    g_cursor_saved = 0;
}

// ===== Рабочий стол =====
void gui_draw_desktop(void) {
    // Стереть старый курсор (если был)
    gui_restore_under_cursor();
    
    gfx_clear_screen(COLOR_BG);
    
    // Панель сверху
    gfx_fill_rect(0, 0, g_width, 24, COLOR_PANEL);
    gfx_draw_char(8, 8, font_A, 2, COLOR_ICON_TXT);
    gfx_draw_char(8 + 12, 8, font_u, 2, COLOR_ICON_TXT);
    gfx_draw_char(8 + 24, 8, font_r, 2, COLOR_ICON_TXT);
    gfx_draw_char(8 + 36, 8, font_a, 2, COLOR_ICON_TXT);
    gfx_draw_char(8 + 48, 8, font_O, 2, COLOR_ICON_TXT);
    gfx_draw_char(8 + 60, 8, font_S, 2, COLOR_ICON_TXT);
    
    // Иконки
    draw_icon(20, 60,  font_F, g_desktop_selected == 0);
    draw_icon(20, 140, font_T, g_desktop_selected == 1);
    draw_icon(20, 220, font_S, g_desktop_selected == 2);
    
    // === ПРАВИЛЬНЫЙ ПОРЯДОК: сначала сохранить фон, потом рисовать курсор ===
    gui_save_under_cursor();   // ← фон под новым местом
    gui_draw_cursor();          // ← теперь рисуем курсор
    
    // Показать всё
    gfx_present();
}

// ===== Окно =====
void gui_draw_window(void) {
    gui_restore_under_cursor();
    
    UINT32 win_x = g_width / 4;
    UINT32 win_y = g_height / 4;
    UINT32 win_w = g_width / 2;
    UINT32 win_h = g_height / 2;
    
    gfx_fill_rect(win_x, win_y, win_w, win_h, COLOR_WIN_BG);
    gfx_fill_rect(win_x, win_y, win_w, 30, COLOR_WIN_BAR);
    gfx_draw_rect_border(win_x, win_y, win_w, win_h, 2, COLOR_WIN_BRD);
    
    if (g_desktop_selected == 0) {
        gfx_draw_char(win_x + 10, win_y + 8, font_F, 2, COLOR_ICON_TXT);
        gfx_draw_char(win_x + 20, win_y + 50, font_F, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 18, win_y + 50, font_i, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 36, win_y + 50, font_e, 3, COLOR_WIN_BAR);
    } else if (g_desktop_selected == 1) {
        gfx_draw_char(win_x + 10, win_y + 8, font_T, 2, COLOR_ICON_TXT);
        gfx_draw_char(win_x + 20, win_y + 50, font_T, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 18, win_y + 50, font_e, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 36, win_y + 50, font_r, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 54, win_y + 50, font_M, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 72, win_y + 50, font_i, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 90, win_y + 50, font_n, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 108, win_y + 50, font_a, 3, COLOR_WIN_BAR);
    } else {
        gfx_draw_char(win_x + 10, win_y + 8, font_S, 2, COLOR_ICON_TXT);
        gfx_draw_char(win_x + 20, win_y + 50, font_S, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 18, win_y + 50, font_e, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 36, win_y + 50, font_t, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 54, win_y + 50, font_t, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 72, win_y + 50, font_i, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 90, win_y + 50, font_n, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 108, win_y + 50, font_g, 3, COLOR_WIN_BAR);
    }
    
    // Порядок: сохранить фон → нарисовать курсор
    gui_save_under_cursor();
    gui_draw_cursor();
    
    gfx_present();
}

// ===== Клавиши =====
void gui_handle_up(void) {
    if (g_window_open) return;
    if (g_desktop_selected > 0) {
        g_desktop_selected--;
        gui_draw_desktop();
    }
}

void gui_handle_down(void) {
    if (g_window_open) return;
    if (g_desktop_selected < 2) {
        g_desktop_selected++;
        gui_draw_desktop();
    }
}

void gui_handle_enter(void) {
    if (g_window_open) return;
    g_window_open = 1;
    gui_draw_window();
}

void gui_handle_esc(void) {
    if (!g_window_open) return;
    g_window_open = 0;
    gui_draw_desktop();
}

void gui_main(void) {
    g_desktop_selected = 0;
    g_window_open = 0;
    gui_draw_desktop();
}