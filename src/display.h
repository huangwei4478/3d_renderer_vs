#pragma once

#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

// screen dimension constants
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern bool is_running;
extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;

bool initialize_window(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_grid(void);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void clear_color_buffer(uint32_t color);
void render_color_buffer(void);
void destroy_window(void);



