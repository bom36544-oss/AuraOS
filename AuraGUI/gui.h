#ifndef GUI_H
#define GUI_H

// ===== Внешние переменные =====
typedef unsigned int UINT32;
typedef unsigned long long UINTN;

extern UINT32 *g_fb;
extern UINT32 g_width;
extern UINT32 g_height;
extern UINT32 g_pitch;
extern int g_mouse_x;
extern int g_mouse_y;

// ===== AuraGUI API =====
void gui_main(void);
void gui_draw_desktop(void);
void gui_draw_window(void);
void gui_handle_up(void);
void gui_handle_down(void);
void gui_handle_enter(void);
void gui_handle_esc(void);

// ===== Шрифты (из font.c) =====
extern const unsigned char font_A[7];
extern const unsigned char font_u[7];
extern const unsigned char font_r[7];
extern const unsigned char font_a[7];
extern const unsigned char font_B[7];
extern const unsigned char font_o[7];
extern const unsigned char font_t[7];
extern const unsigned char font_S[7];
extern const unsigned char font_f[7];
extern const unsigned char font_e[7];
extern const unsigned char font_M[7];
extern const unsigned char font_d[7];
extern const unsigned char font_O[7];
extern const unsigned char font_i[7];
extern const unsigned char font_n[7];
extern const unsigned char font_g[7];
extern const unsigned char font_F[7];
extern const unsigned char font_T[7];
extern const unsigned char font_E[7];
extern const unsigned char font_space[7];

#endif