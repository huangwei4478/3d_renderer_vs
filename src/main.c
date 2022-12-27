#include <SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "display.h"
#include "vector.h"

void setup(void) {
	color_buffer = (uint32_t*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
	color_buffer_texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH,
		SCREEN_HEIGHT);
}

void process_input(void) {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: {				// click "x" button of the window
			is_running = false;
			break;
		}
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				is_running = false;
			}
			break;
		}
		default: {
			break;
		}
		}
	}
}

void update(void) {
	// TODO:
}

void render(void) {
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	draw_grid();
	draw_rect(100, 100, 100, 150, 0xFFFF0000);
	render_color_buffer();
	clear_color_buffer(0xFFFFFF00);

	SDL_RenderPresent(renderer);
}

int main(void) {
	is_running = initialize_window();

	setup();

	while (is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();
	return 0;
}
