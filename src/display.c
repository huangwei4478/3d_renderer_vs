#include "display.h"
#include <stdio.h>

// Screen dimension constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;


bool initialize_window(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "failed to initialize SDL2.");
		return false;
	}

	window = SDL_CreateWindow("3D Graphics Programming",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);

	if (window == NULL) {
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL) {
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}

	// This line would make my macbook system-hang.
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	return true;
}

void draw_pixel(int x, int y, uint32_t color) {
	SDL_assert(x >= 0 && y >= 0 && x < SCREEN_WIDTH&& y < SCREEN_HEIGHT);
	color_buffer[SCREEN_WIDTH * y + x] = color;
}

void draw_grid(void) {
	// Exercise: Draw a background grid that fills the entire window
	// Lines should be rendered at every row/col multiple of 10.

	const int grid_wh = 10;

	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			if (x % grid_wh == 0 || y % grid_wh == 0) {
				draw_pixel(x, y, 0xFF000000);
			}
		}
	}
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
	// use DDA algorithm to draw a line
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);
	
	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;

	for (int i = 0; i < longest_side_length; i++) {
		draw_pixel(round(current_x), round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
	SDL_assert(x >= 0 && y >= 0 && x + width < SCREEN_WIDTH && y + height < SCREEN_HEIGHT);
	for (int _y = y; _y < y + height; _y++) {
		for (int _x = x; _x < x + width; _x++) {
			draw_pixel(_x, _y, color);
		}
	}
}

void clear_color_buffer(uint32_t color) {
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			draw_pixel(x, y, color);
		}
	}
}

void render_color_buffer(void) {
	SDL_UpdateTexture(color_buffer_texture,
		NULL,
		color_buffer,
		(int)(SCREEN_WIDTH * sizeof(uint32_t)));
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void destroy_window(void) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
