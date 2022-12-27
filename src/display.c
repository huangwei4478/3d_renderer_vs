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

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);

	SCREEN_WIDTH = displayMode.w;
	SCREEN_HEIGHT = displayMode.h;

	window = SDL_CreateWindow("3D Graphics Programming",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		0);

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

void draw_rect(int x, int y, int width, int height, uint32_t color) {
	SDL_assert(x < 0 || y < 0 || x + width >= SCREEN_WIDTH || y + height >= SCREEN_HEIGHT);
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
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
