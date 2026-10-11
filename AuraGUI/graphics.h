#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef unsigned int UINT32;

extern UINT32 *g_fb;
extern UINT32 g_width;
extern UINT32 g_height;
extern UINT32 g_pitch;

void gfx_put_pixel(UINT32 x, UINT32 y, UINT32 color);
void gfx_fill_rect(UINT32 x, UINT32 y, UINT32 w, UINT32 h, UINT32 color);
void gfx_draw_rect_border(UINT32 x, UINT32 y, UINT32 w, UINT32 h, UINT32 t, UINT32 color);
void gfx_clear_screen(UINT32 color);
void gfx_draw_char(UINT32 x, UINT32 y, const unsigned char *font, UINT32 scale, UINT32 color);

#endif