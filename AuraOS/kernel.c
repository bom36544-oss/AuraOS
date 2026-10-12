// ===== AuraOS — ядро (boot + меню + мышь + клики) =====

#include "kernel.h"
#include "../AuraGUI/gui.h"
#include "../AuraGUI/graphics.h"

typedef unsigned short CHAR16;
typedef unsigned long long EFI_STATUS;
typedef unsigned int UINT32;
typedef int INT32;
typedef unsigned long long UINTN;

extern void *g_conin;
extern int g_mouse_x;
extern int g_mouse_y;

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

// ===== EFI_SIMPLE_POINTER (INT32!) =====
typedef struct {
    INT32 RelativeMovementX;
    INT32 RelativeMovementY;
    INT32 RelativeMovementZ;
    unsigned char LeftButton;
    unsigned char RightButton;
} EFI_SIMPLE_POINTER_STATE;

typedef struct {
    void *Reset;
    EFI_STATUS (*GetState)(void *This, EFI_SIMPLE_POINTER_STATE *State);
    void *WaitForInput;
} EFI_SIMPLE_POINTER_PROTOCOL;

extern EFI_SIMPLE_POINTER_PROTOCOL *g_mouse;

// ===== НОВОЕ: состояние ЛКМ и колбэк =====
extern unsigned char g_mouse_left;
extern unsigned char g_mouse_left_prev;
extern void gui_handle_click(int x, int y);

#define COLOR_BLACK 0x00000000
#define COLOR_WHITE 0x00FFFFFF

void delay(UINTN count) {
    for (volatile UINTN i = 0; i < count; i++);
}

static void draw_logo(UINT32 x, UINT32 y, UINT32 scale, UINT32 color) {
    UINT32 step = 6 * scale;
    gfx_draw_char(x + 0 * step, y, font_A, scale, color);
    gfx_draw_char(x + 1 * step, y, font_u, scale, color);
    gfx_draw_char(x + 2 * step, y, font_r, scale, color);
    gfx_draw_char(x + 3 * step, y, font_a, scale, color);
    gfx_draw_char(x + 4 * step, y, font_space, scale, color);
    gfx_draw_char(x + 5 * step, y, font_B, scale, color);
    gfx_draw_char(x + 6 * step, y, font_o, scale, color);
    gfx_draw_char(x + 7 * step, y, font_o, scale, color);
    gfx_draw_char(x + 8 * step, y, font_t, scale, color);
}

static int g_selected = 0;

static void draw_menu(UINT32 scale) {
    UINT32 step = 6 * scale;
    UINT32 line_h = 10 * scale;
    UINT32 start_y = g_height / 2;
    UINT32 start_x = g_width / 3;
    
    gfx_fill_rect(start_x - 4*scale, start_y - 4*scale, 
                  g_width/2 + 8*scale, line_h * 2 + 12*scale, COLOR_BLACK);
    
    if (g_selected == 0) {
        gfx_fill_rect(start_x - 2*scale, start_y - 2*scale, g_width/2 + 4*scale, line_h, COLOR_WHITE);
        gfx_draw_char(start_x, start_y, font_A, scale, COLOR_BLACK);
        gfx_draw_char(start_x + step, start_y, font_u, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 2*step, start_y, font_r, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 3*step, start_y, font_a, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 4*step, start_y, font_O, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 5*step, start_y, font_S, scale, COLOR_BLACK);
    } else {
        gfx_draw_char(start_x, start_y, font_A, scale, COLOR_WHITE);
        gfx_draw_char(start_x + step, start_y, font_u, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 2*step, start_y, font_r, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 3*step, start_y, font_a, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 4*step, start_y, font_O, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 5*step, start_y, font_S, scale, COLOR_WHITE);
    }
    
    if (g_selected == 1) {
        gfx_fill_rect(start_x - 2*scale, start_y + line_h - 2*scale, g_width/2 + 4*scale, line_h, COLOR_WHITE);
        gfx_draw_char(start_x, start_y + line_h, font_S, scale, COLOR_BLACK);
        gfx_draw_char(start_x + step, start_y + line_h, font_a, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 2*step, start_y + line_h, font_f, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 3*step, start_y + line_h, font_e, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 4*step, start_y + line_h, font_space, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 5*step, start_y + line_h, font_M, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 6*step, start_y + line_h, font_o, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 7*step, start_y + line_h, font_d, scale, COLOR_BLACK);
        gfx_draw_char(start_x + 8*step, start_y + line_h, font_e, scale, COLOR_BLACK);
    } else {
        gfx_draw_char(start_x, start_y + line_h, font_S, scale, COLOR_WHITE);
        gfx_draw_char(start_x + step, start_y + line_h, font_a, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 2*step, start_y + line_h, font_f, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 3*step, start_y + line_h, font_e, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 4*step, start_y + line_h, font_space, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 5*step, start_y + line_h, font_M, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 6*step, start_y + line_h, font_o, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 7*step, start_y + line_h, font_d, scale, COLOR_WHITE);
        gfx_draw_char(start_x + 8*step, start_y + line_h, font_e, scale, COLOR_WHITE);
    }
    
    gfx_present();
}

static void draw_booting(void) {
    UINT32 scale = 4;
    UINT32 step = 6 * scale;
    gfx_clear_screen(COLOR_BLACK);
    
    UINT32 msg_x = (g_width - 7 * step) / 2;
    UINT32 msg_y = g_height / 2 - 4 * scale;
    
    gfx_draw_char(msg_x + 0 * step, msg_y, font_B, scale, COLOR_WHITE);
    gfx_draw_char(msg_x + 1 * step, msg_y, font_o, scale, COLOR_WHITE);
    gfx_draw_char(msg_x + 2 * step, msg_y, font_o, scale, COLOR_WHITE);
    gfx_draw_char(msg_x + 3 * step, msg_y, font_t, scale, COLOR_WHITE);
    gfx_draw_char(msg_x + 4 * step, msg_y, font_i, scale, COLOR_WHITE);
    gfx_draw_char(msg_x + 5 * step, msg_y, font_n, scale, COLOR_WHITE);
    gfx_draw_char(msg_x + 6 * step, msg_y, font_g, scale, COLOR_WHITE);
    
    UINT32 dot_y = msg_y + 9 * scale;
    UINT32 dot_x = (g_width - 5 * scale) / 2;
    gfx_fill_rect(dot_x + 0 * scale, dot_y, scale, scale, COLOR_WHITE);
    gfx_fill_rect(dot_x + 2 * scale, dot_y, scale, scale, COLOR_WHITE);
    gfx_fill_rect(dot_x + 4 * scale, dot_y, scale, scale, COLOR_WHITE);
    
    gfx_present();
}

void kernel_main(void) {
    UINT32 scale = 4;
    UINT32 logo_w = 9 * 6 * scale;
    UINT32 logo_h = 7 * scale;
    UINT32 start_x = (g_width - logo_w) / 2;
    UINT32 final_y = 150;
    
    gfx_clear_screen(COLOR_BLACK);
    gfx_present();
    
    // ===== Анимация 5 сек =====
    UINT32 total_frames = 50;
    UINT32 y_start = g_height - logo_h;
    UINT32 delta = (y_start - final_y) / total_frames;
    if (delta < 1) delta = 1;
    
    UINT32 prev_y = y_start;
    draw_logo(start_x, y_start, scale, COLOR_WHITE);
    gfx_present();
    delay(3000000);
    
    for (UINT32 i = 1; i < total_frames; i++) {
        UINT32 y = y_start - i * delta;
        if (y <= final_y || i == total_frames - 1) y = final_y;
        
        gfx_fill_rect(start_x, prev_y, logo_w, logo_h, COLOR_BLACK);
        draw_logo(start_x, y, scale, COLOR_WHITE);
        gfx_present();
        
        prev_y = y;
        delay(3000000);
        
        if (y == final_y) break;
    }
    
    draw_logo(start_x, final_y, scale, COLOR_WHITE);
    draw_menu(scale);
    
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *conin = 
        (EFI_SIMPLE_TEXT_INPUT_PROTOCOL *)g_conin;
    
    int in_gui = 0;
    
    while (1) {
        // ===== Мышь =====
        if (in_gui && g_mouse != 0) {
            EFI_SIMPLE_POINTER_STATE ms;
            EFI_STATUS st_ms = g_mouse->GetState(g_mouse, &ms);
            
            if (st_ms == 0) {
                long long dx = ms.RelativeMovementX;
                long long dy = ms.RelativeMovementY;
                
                // ===== 1. ДВИЖЕНИЕ КУРСОРА =====
                if (dx != 0 || dy != 0) {
                    extern void gui_restore_under_cursor(void);
                    extern void gui_draw_cursor(void);
                    extern void gui_save_under_cursor(void);
                    extern void gfx_present_rect(UINT32, UINT32, UINT32, UINT32);
                    
                    gui_restore_under_cursor();
                    
                    g_mouse_x += (int)dx;
                    g_mouse_y += (int)dy;
                    
                    if (g_mouse_x < 0) g_mouse_x = 0;
                    if (g_mouse_y < 0) g_mouse_y = 0;
                    if (g_mouse_x > (int)g_width - 24) g_mouse_x = (int)g_width - 24;
                    if (g_mouse_y > (int)g_height - 40) g_mouse_y = (int)g_height - 40;
                    
                    gui_save_under_cursor();
                    gui_draw_cursor();
                    gfx_present_rect(g_mouse_x, g_mouse_y, 24, 40);
                }
                
                // ===== 2. КЛИК ЛКМ =====
                g_mouse_left = ms.LeftButton ? 1 : 0;
                
                // Фронт: было 0 → стало 1 = новое нажатие
                if (g_mouse_left == 1 && g_mouse_left_prev == 0) {
                    gui_handle_click(g_mouse_x, g_mouse_y);
                }
                
                g_mouse_left_prev = g_mouse_left;
            }
        }
        
        // ===== Клавиатура =====
        EFI_INPUT_KEY key;
        EFI_STATUS st = conin->ReadKeyStroke(conin, &key);
        
        if (st == 0) {
            if (!in_gui) {
                if (key.ScanCode == 0x01) {
                    if (g_selected != 0) { g_selected = 0; draw_menu(scale); }
                }
                else if (key.ScanCode == 0x02) {
                    if (g_selected != 1) { g_selected = 1; draw_menu(scale); }
                }
                else if (key.UnicodeChar == 0x0D) {
                    draw_booting();
                    delay(100000000);
                    gui_main();
                    in_gui = 1;
                }
            } else {
                if (key.ScanCode == 0x01) gui_handle_up();
                else if (key.ScanCode == 0x02) gui_handle_down();
                else if (key.UnicodeChar == 0x0D) gui_handle_enter();
                else if (key.ScanCode == 0x17) gui_handle_esc();
            }
        }
        
        delay(50000);
    }
}