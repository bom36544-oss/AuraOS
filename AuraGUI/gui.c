// ===== AuraGUI — рабочий стол + окна =====

#include "gui.h"
#include "graphics.h"

// ===== Цвета =====
#define COLOR_BG       0x00202020
#define COLOR_PANEL    0x00404040
#define COLOR_ICON_BG  0x00606060
#define COLOR_ICON_SEL 0x00A0A0A0
#define COLOR_ICON_TXT 0x00FFFFFF
#define COLOR_WIN_BG   0x00E0E0E0
#define COLOR_WIN_BAR  0x00505050
#define COLOR_WIN_BRD  0x00FFFFFF

// ===== Состояние GUI =====
static int g_desktop_selected = 0;
static int g_window_open = 0;

// ===== Иконка =====
static void draw_icon(UINT32 x, UINT32 y, const unsigned char *letter, int selected) {
    UINT32 size = 64;
    UINT32 border = 2;
    UINT32 bg = selected ? COLOR_ICON_SEL : COLOR_ICON_BG;
    
    gfx_fill_rect(x, y, size, size, bg);
    gfx_draw_rect_border(x, y, size, size, border, COLOR_ICON_TXT);
    
    UINT32 char_scale = 6;
    UINT32 char_w = 5 * char_scale;
    UINT32 char_h = 7 * char_scale;
    gfx_draw_char(x + (size - char_w) / 2, y + (size - char_h) / 2, letter, char_scale, COLOR_ICON_TXT);
}

// ===== Курсор «A» =====
static void draw_cursor(void) {
    // g_mouse_x, g_mouse_y — координаты
    gfx_draw_char(g_mouse_x, g_mouse_y, font_A, 4, 0x00FFFFFF);
    // Маленькая точка (прицел)
    gfx_fill_rect(g_mouse_x + 8, g_mouse_y + 32, 8, 2, 0x00FFFFFF);
    gfx_fill_rect(g_mouse_x + 11, g_mouse_y + 29, 2, 8, 0x00FFFFFF);
}

// ===== Рабочий стол =====
void gui_draw_desktop(void) {
    gfx_clear_screen(COLOR_BG);
    
    // Панель сверху
    gfx_fill_rect(0, 0, g_width, 24, COLOR_PANEL);
    gfx_draw_char(8, 8, font_A, 2, COLOR_ICON_TXT);
    gfx_draw_char(8 + 12, 8, font_u, 2, COLOR_ICON_TXT);
    gfx_draw_char(8 + 24, 8, font_r, 2, COLOR_ICON_TXT);
    gfx_draw_char(8 + 36, 8, font_a, 2, COLOR_ICON_TXT);
    gfx_draw_char(8 + 48, 8, font_O, 2, COLOR_ICON_TXT);
    gfx_draw_char(8 + 60, 8, font_S, 2, COLOR_ICON_TXT);
    
    // Иконки слева
    draw_icon(20, 60,  font_F, g_desktop_selected == 0);
    draw_icon(20, 140, font_T, g_desktop_selected == 1);
    draw_icon(20, 220, font_S, g_desktop_selected == 2);
    
    // Курсор «A»
    draw_cursor();
}

// ===== Окно =====
void gui_draw_window(void) {
    UINT32 win_x = g_width / 4;
    UINT32 win_y = g_height / 4;
    UINT32 win_w = g_width / 2;
    UINT32 win_h = g_height / 2;
    
    gfx_fill_rect(win_x, win_y, win_w, win_h, COLOR_WIN_BG);
    gfx_fill_rect(win_x, win_y, win_w, 30, COLOR_WIN_BAR);
    gfx_draw_rect_border(win_x, win_y, win_w, win_h, 2, COLOR_WIN_BRD);
    
    // Заголовок
    if (g_desktop_selected == 0) {
        gfx_draw_char(win_x + 10, win_y + 8, font_F, 2, COLOR_ICON_TXT);
    } else if (g_desktop_selected == 1) {
        gfx_draw_char(win_x + 10, win_y + 8, font_T, 2, COLOR_ICON_TXT);
    } else {
        gfx_draw_char(win_x + 10, win_y + 8, font_S, 2, COLOR_ICON_TXT);
    }
    
    // Кнопка закрытия
    UINT32 close_x = win_x + win_w - 30;
    gfx_fill_rect(close_x + 4, win_y + 8, 16, 2, COLOR_ICON_TXT);
    gfx_fill_rect(close_x + 11, win_y + 4, 2, 16, COLOR_ICON_TXT);
    
    // Текст
    UINT32 text_y = win_y + 50;
    if (g_desktop_selected == 0) {
        gfx_draw_char(win_x + 20, text_y, font_F, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 18, text_y, font_i, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 36, text_y, font_e, 3, COLOR_WIN_BAR);
    } else if (g_desktop_selected == 1) {
        gfx_draw_char(win_x + 20, text_y, font_T, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 18, text_y, font_e, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 36, text_y, font_r, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 54, text_y, font_M, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 72, text_y, font_i, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 90, text_y, font_n, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 108, text_y, font_a, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 126, text_y, font_space, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 144, text_y, font_O, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 162, text_y, font_S, 3, COLOR_WIN_BAR);
    } else {
        gfx_draw_char(win_x + 20, text_y, font_S, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 18, text_y, font_e, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 36, text_y, font_t, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 54, text_y, font_t, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 72, text_y, font_i, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 90, text_y, font_n, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 108, text_y, font_g, 3, COLOR_WIN_BAR);
        gfx_draw_char(win_x + 20 + 126, text_y, font_S, 3, COLOR_WIN_BAR);
    }
}

// ===== Обработка клавиш =====
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

// ===== Точка входа AuraGUI =====
void gui_main(void) {
    g_desktop_selected = 0;
    g_window_open = 0;
    gui_draw_desktop();
}

// ===== Перерисовка курсора (вызывать при движении мыши) =====
void gui_redraw_cursor(void) {
    if (g_window_open) {
        gui_draw_window();
    } else {
        gui_draw_desktop();
    }
}