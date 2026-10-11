#include "graphics.h"

void gfx_init(void) {}

// Копировать ВСЁ back → fb (на экран)
void gfx_present(void) {
    if (!g_back || !g_fb) return;
    for (UINT32 y = 0; y < g_height; y++) {
        for (UINT32 x = 0; x < g_width; x++) {
            g_fb[y * g_pitch + x] = g_back[y * g_pitch + x];
        }
    }
}

// Копировать ТОЛЬКО область (x, y, w, h) из back → fb
void gfx_present_rect(UINT32 x, UINT32 y, UINT32 w, UINT32 h) {
    if (!g_back || !g_fb) return;
    if (x >= g_width || y >= g_height) return;
    if (x + w > g_width) w = g_width - x;
    if (y + h > g_height) h = g_height - y;
    
    for (UINT32 j = 0; j < h; j++) {
        for (UINT32 i = 0; i < w; i++) {
            g_fb[(y + j) * g_pitch + (x + i)] = g_back[(y + j) * g_pitch + (x + i)];
        }
    }
}

void gfx_put_pixel(UINT32 x, UINT32 y, UINT32 color) {
    if (x >= g_width || y >= g_height) return;
    if (!g_back) return;
    g_back[y * g_pitch + x] = color;
}

void gfx_fill_rect(UINT32 x, UINT32 y, UINT32 w, UINT32 h, UINT32 color) {
    for (UINT32 j = 0; j < h; j++) {
        for (UINT32 i = 0; i < w; i++) {
            gfx_put_pixel(x + i, y + j, color);
        }
    }
}

void gfx_draw_rect_border(UINT32 x, UINT32 y, UINT32 w, UINT32 h, UINT32 t, UINT32 color) {
    gfx_fill_rect(x, y, w, t, color);
    gfx_fill_rect(x, y + h - t, w, t, color);
    gfx_fill_rect(x, y, t, h, color);
    gfx_fill_rect(x + w - t, y, t, h, color);
}

void gfx_clear_screen(UINT32 color) {
    if (!g_back) return;
    for (UINT32 y = 0; y < g_height; y++) {
        for (UINT32 x = 0; x < g_width; x++) {
            g_back[y * g_pitch + x] = color;
        }
    }
}

void gfx_draw_char(UINT32 x, UINT32 y, const unsigned char *font, UINT32 scale, UINT32 color) {
    for (UINT32 row = 0; row < 7; row++) {
        for (UINT32 col = 0; col < 5; col++) {
            if (font[row] & (1 << (4 - col))) {
                gfx_fill_rect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}